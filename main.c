#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

typedef struct
{
	float	x;
	float	y;
	float	z;
} vec_t;

static const vec_t vec_zero = { 0, 0, 0 };
static const vec_t vec_unit = { 1, 1, 1 };

static inline void vec_cross(vec_t *r, const vec_t *a, const vec_t *b)
{
	vec_t v;

	v.x = a->y * b->z - a->z * b-> y;
	v.y = a->z * b->x - a->x * b-> z;
	v.z = a->x * b->y - a->y * b-> x;

	*r = v;
}

static inline float vec_dot(const vec_t *a, const vec_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

static inline void vec_scale(vec_t *r, float s, const vec_t *a)
{
	r->x = s * a->x;
	r->y = s * a->y;
	r->z = s * a->z;
}

static inline float vec_len_sq(const vec_t *a)
{
	return vec_dot(a, a);
}

static inline float vec_len(const vec_t *a)
{
	return sqrtf(vec_len_sq(a));
}

static inline void vec_norm(vec_t *r, const vec_t *a)
{
	vec_scale(r, 1 / vec_len(a), a);
}

static inline void vec_add(vec_t *r, const vec_t *a, const vec_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

static inline void vec_cpy(vec_t *r, const vec_t *a)
{
	*r = *a;
}

static inline void vec_fma(vec_t *r, const vec_t *a, float s, const vec_t *b)
{
	r->x = a->x + s * b->x;
	r->y = a->y + s * b->y;
	r->z = a->z + s * b->z;
}

static inline void vec_sub(vec_t *r, const vec_t *a, const vec_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
}

static inline void vec_nmul(vec_t *r, const vec_t *a, const vec_t *b)
{
	r->x = a->x * b->x;
	r->y = a->y * b->y;
	r->z = a->z * b->z;
}

static inline void vec_clamp(vec_t *r, const vec_t *a,
				const vec_t *lo, const vec_t *hi)
{
	r->x = (a->x > lo->x ? a->x < hi->x ? a->x : hi->x : lo->x);
	r->y = (a->y > lo->y ? a->y < hi->y ? a->y : hi->y : lo->y);
	r->z = (a->z > lo->z ? a->z < hi->z ? a->z : hi->z : lo->z);
}

static inline float vec_cos(const vec_t *a, const vec_t *b)
{
	return vec_dot(a, b) / (vec_len(a) * vec_len(b));
}

static inline float vec_proj_s(const vec_t *a, const vec_t *b)
{
	return vec_dot(a, b) / vec_len(b);
}

static inline void vec_proj_v(vec_t *r, const vec_t *a, const vec_t *b)
{
	vec_scale(r, vec_dot(a, b) / vec_len_sq(b), b);
}

static inline void vec_rej_v(vec_t *r, const vec_t *a, const vec_t *b)
{
	vec_t v;

	vec_proj_v(&v, a, b);
	vec_sub(r, a, &v);
}


typedef struct
{
	vec_t	p;
	vec_t	uv;
	vec_t	fv;
	float	l;
	float	r;
	float	t;
	float	b;
	float	n;
} cam_t;

typedef struct
{
	int	w;
	int	h;
	float	ulx;
	float	uly;
} vp_t;


typedef struct
{
	vec_t	a;
	vec_t	b;
	vec_t	c;
	vec_t	col;
} tri_t;

typedef struct
{
	vec_t	c;
	float	r;
} sph_t;

static int ray(vec_t *c, vec_t *p, vec_t *d, int depth);

static int ray_tri(tri_t* tri, vec_t *c, vec_t *p, vec_t *d,
			float *dist, int depth)
{
	vec_t	n;
	vec_t	q;
	float	s;
	float	t;
	vec_t	i;
	vec_t	j;

	{
		vec_t	a;
		vec_t	b;

		vec_sub(&a, &tri->a, &tri->c);
		vec_sub(&b, &tri->b, &tri->c);

		vec_cross(&n, &b, &a);
		vec_norm(&n, &n);

		s = vec_dot(&tri->a, &n);
	}

	t = vec_dot(d, &n);
	if (t <= 0)
	{
		/* Surface is facing away */
		return 0;
	}

	t = (s - vec_dot(p, &n)) / t;
	if (t <= 0)
	{
		/* Surface is behind ray origin */
		return 0;
	}
	else if (t >= *dist)
	{
		/* Surface is occluded */
		return 0;
	}

	vec_fma(&q, p, t, d);

	vec_sub(&i, &tri->a, &tri->c);
	vec_norm(&i, &i);
	vec_cross(&j, &i, &n);

	{
		float	x_1	= vec_dot(&tri->a, &i);
		float	y_1	= vec_dot(&tri->a, &j);
		float	x_2	= vec_dot(&tri->b, &i);
		float	y_2	= vec_dot(&tri->b, &j);
		float	x_3	= vec_dot(&tri->c, &i);
		float	y_3	= vec_dot(&tri->c, &j);
		float	x	= vec_dot(&q, &i);
		float	y	= vec_dot(&q, &j);
		float	det	=	(y_2 - y_3) * (x_1 - x_3)	+
					(x_3 - x_2) * (y_1 - y_3)	;
		float	l_1	= (	(y_2 - y_3) * (x   - x_3)	+
					(x_3 - x_2) * (y   - y_3)	) / det;
		float	l_2	= (	(y_3 - y_1) * (x   - x_3)	+
					(x_1 - x_3) * (y   - y_3)	) / det;
		float	l_3	= 1 - l_1 - l_2;

		if (	(l_1 >= 0 && l_1 <= 1)	&&
			(l_2 >= 0 && l_2 <= 1)	&&
			(l_3 >= 0 && l_3 <= 1)	)
		{
			float m = 100;
			float a = m / (t * t);
			vec_t v = tri->col;

			if (v.x == -1)
			{
				v.x = l_1;
				v.y = l_2;
				v.z = l_3;
			}

			if (a > 1) a = 1;

			/* Reflect */
			vec_t rd;
			vec_fma(&rd, d, -2 * vec_dot(d, &n), &n);

			vec_cpy(c, &vec_zero);
			ray(c, &q, &rd, depth);

			/* Reflectivity */
			vec_scale(c, 0.5, c);
			/* Ambient */
			vec_fma(c, c, 0.2, &vec_unit);
			/* Texture */
			vec_nmul(c, c, &v);
			/* Attenuation */
			vec_scale(c, a, c);

			*dist = t;

			return 1;
		}
	}

	return 0;
}

static int ray_p_light(sph_t *light, vec_t *c, vec_t *p, vec_t *d,
			float *dist, int depth)
{
	vec_t o;
	vec_t v;
	vec_t w;
	float s;
	float t;

	vec_sub(&o, &light->c, p);
	s = vec_dot(&o, d);

	if (s <= 0)
	{
		/* Light is behind ray origin */
		return 0;
	}
	else if (s >= *dist)
	{
		/* Light is occluded */
		return 0;
	}

	vec_scale(&v, s, d);
	vec_sub(&w, &v, &o);
	t = vec_len(&w);

	if (t > light->r)
	{
		/* Ray intersection is outside light radius */
		return 0;
	}

	{
		float m = 100;
		float a;

		a = sqrtf((light->r - t) / light->r);
		s = s - a * light->r;
		a = m * a / (s * s);

		vec_fma(c, c, a, &vec_unit);

		return 1;
	}
}

static tri_t tri_list[] =
{
	/* Right */
	{
		{  10,  10,  10 },
		{  10,  10, -10 },
		{  10, -10, -10 },
		{   0,   1,   1 },
	},
	{
		{  10,  10,  10 },
		{  10, -10, -10 },
		{  10, -10,  10 },
		{   0,   1,   1 },
	},
	/* Left */
	{
		{ -10,  10, -10 },
		{ -10,  10,  10 },
		{ -10, -10,  10 },
		{   1,   1,   0 },
	},
	{
		{ -10,  10, -10 },
		{ -10, -10,  10 },
		{ -10, -10, -10 },
		{   1,   1,   0 },
	},
	/* Top */
	{
		{ -10,  10, -10 },
		{  10,  10, -10 },
		{  10,  10,  10 },
		{   1,   1,   1 },
	},
	{
		{ -10,  10, -10 },
		{  10,  10,  10 },
		{ -10,  10,  10 },
		{   1,   1,   1 },
	},
	/* Bottom */
	{
		{ -10, -10,  10 },
		{  10, -10,  10 },
		{  10, -10, -10 },
		{   1,   1,   1 },
	},
	{
		{ -10, -10,  10 },
		{  10, -10, -10 },
		{ -10, -10, -10 },
		{   1,   1,   1 },
	},
	/* Front */
	{
		{ -10,  10,  10 },
		{  10,  10,  10 },
		{  10, -10,  10 },
		{   1,   0,  1  },
	},
	{
		{ -10,  10,  10 },
		{  10, -10,  10 },
		{ -10, -10,  10 },
		{   1,   0,  1  },
	},
	/* Back */
	{
		{  10,  10, -10 },
		{ -10,  10, -10 },
		{ -10, -10, -10 },
		{   0,   0,  1  },
	},
	{
		{  10,  10, -10 },
		{ -10, -10, -10 },
		{  10, -10, -10 },
		{   0,   0,  1  },
	},

	{
		{ -1, -1,  2 },
		{  0,  1,  2 },
		{  1, -1,  2 },
		{ -1, -1, -1 },
	},
};

static sph_t p_lights[] =
{
	{
		{ -2,  0, -2 },
		2,
	}
};

static int ray(vec_t *c, vec_t *p, vec_t *d, int depth)
{
	float dist = 1. / 0.;

	if (depth == 4)
	{
		return 0;
	}

	for (int i = 0; i < sizeof(tri_list) / sizeof*(tri_list); i++)
	{
		tri_t tri = tri_list[i];

		ray_tri(&tri, c, p, d, &dist, depth + 1);
	}

	for (int i = 0; i < sizeof(p_lights) / sizeof*(p_lights); i++)
	{
		sph_t light = p_lights[i];

		ray_p_light(&light, c, p, d, &dist, depth + 1);
	}

	return 0;
}

#define WIDTH	800
#define HEIGHT	600

static GLuint	rt_fbo;
static GLuint	rt_tex;
static char	rt_buf[WIDTH * HEIGHT * 3];

static int rt(cam_t *cam, vp_t *vp)
{
	vec_t	p	= cam->p;
	vec_t	rv	= { 0, 0, 0 };
	vec_t	uv	= cam->uv;
	vec_t	fv	= cam->fv;
	float	l	= cam->l;
	float	r	= cam->r;
	float	t	= cam->t;
	float	b	= cam->b;
	float	n	= cam->n;
	int	w	= vp->w;
	int	h	= vp->h;
	float	ulx	= vp->ulx;
	float	uly	= vp->uly;

	vec_cross(&rv, &uv, &fv);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			int i = (y * w + x) * 3;
			vec_t d;
			vec_t c;
			float rs = ulx + x * (r - l) / (w - 1);
			float us = uly + y * (t - b) / (h - 1);
			float fs = n;

			vec_fma(&d, &vec_zero, rs, &rv);
			vec_fma(&d, &d, us, &uv);
			vec_fma(&d, &d, fs, &fv);
			vec_norm(&d, &d);

			vec_cpy(&c, &vec_zero);

			ray(&c, &p, &d, 0);

			vec_clamp(&c, &c, &vec_zero, &vec_unit);

			rt_buf[i + 0] = c.x * nextafter(256, 0);
			rt_buf[i + 1] = c.y * nextafter(256, 0);
			rt_buf[i + 2] = c.z * nextafter(256, 0);
		}
	}

	return 0;
}


static float	r;
static float	t;
static cam_t	cam;
static vp_t	vp;
static char	keys[256];

static void display_func(void)
{
	if (keys[GLUT_KEY_LEFT])
	{
		if (keys[GLUT_KEY_CTRL_L])
		{
			cam.p.x = cam.p.x - cam.fv.z * 0.1;
			cam.p.z = cam.p.z + cam.fv.x * 0.1;
		}
		else
		{
			r = r - 0.1;
		}
	}
	if (keys[GLUT_KEY_RIGHT])
	{
		if (keys[GLUT_KEY_CTRL_L])
		{
			cam.p.x = cam.p.x + cam.fv.z * 0.1;
			cam.p.z = cam.p.z - cam.fv.x * 0.1;
		}
		else
		{
			r = r + 0.1;
		}
	}

	cam.fv.z = cosf(r);
	cam.fv.x = sinf(r);

	if (keys[GLUT_KEY_UP])
	{
		vec_fma(&cam.p, &cam.p, 0.1, &cam.fv);
	}
	if (keys[GLUT_KEY_DOWN])
	{
		vec_fma(&cam.p, &cam.p, -0.1, &cam.fv);
	}

	rt(&cam, &vp);

	glBindTexture(GL_TEXTURE_2D, rt_tex);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGB,
			WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE,
			rt_buf);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, rt_fbo);
	glFramebufferTexture2D(	GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, rt_tex, 0);
	glBlitFramebuffer(	0, 0, WIDTH, HEIGHT,
				0, 0, WIDTH, HEIGHT,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	glutSwapBuffers();
	glutPostRedisplay();

	t = t + 0.01;
}

static void keyboard_func(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		glutLeaveMainLoop();
	}
}

static void special_func(int key, int x, int y)
{
	keys[key] = 1;
}

static void special_up_func(int key, int x, int y)
{
	keys[key] = 0;
}

int main(int argc, char *argv[])
{
	cam.p.x = 0;
	cam.p.y = 0;
	cam.p.z = 0;
	cam.uv.x = 0;
	cam.uv.y = 1;
	cam.uv.z = 0;
	cam.fv.x = 0;
	cam.fv.y = 0;
	cam.fv.z = 1;
	cam.l = -4. / 3.;
	cam.r = 4. / 3;
	cam.t = 1;
	cam.b = -1;
	cam.n = 1;

	vp.w = WIDTH;
	vp.h = HEIGHT;
	vp.ulx = -4. / 3.;
	vp.uly = -1.;

	r = 0;

	glutInit(&argc, argv);

	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("rt");
	glewInit();

	glGenTextures(1, &rt_tex);
	glGenFramebuffers(1, &rt_fbo);

	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutSpecialFunc(special_func);
	glutSpecialUpFunc(special_up_func);

	glutMainLoop();
}
