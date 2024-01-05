#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "stb_image.h"
#include "tex.h"
#include "vec.h"

#define BILERP

int tex_load_c(tex_t *tex, const char *filename)
{
	int n;

	tex->c = stbi_load(filename, &tex->w, &tex->h, &n, 3);

	/* Gamma correction */
	if (tex->c != NULL)
	{
		for (int y = 0; y < tex->h; y++)
		{
			for (int x = 0; x < tex->w; x++)
			{
				unsigned char *p = tex->c;

				p = &p[(y * tex->w + x) * 3];

				p[0] = 0.5 + pow(p[0] / 255., 2.2) * 255.;
				p[1] = 0.5 + pow(p[1] / 255., 2.2) * 255.;
				p[2] = 0.5 + pow(p[2] / 255., 2.2) * 255.;
			}
		}
	}

	return tex->c != NULL;
}

int tex_load_n(tex_t *tex, const char *filename)
{
	int n;

	tex->n = stbi_load(filename, &tex->w, &tex->h, &n, 3);

	return tex->n != NULL;
}

int tex_load_r(tex_t *tex, const char *filename)
{
	int n;

	tex->r = stbi_load(filename, &tex->w, &tex->h, &n, 3);

	return tex->r != NULL;
}

void tex_dstr(tex_t *tex)
{
	if (tex->c != NULL)
	{
		free(tex->c);

		tex->c = NULL;
	}

	if (tex->n != NULL)
	{
		free(tex->n);

		tex->n = NULL;
	}

	if (tex->r != NULL)
	{
		free(tex->r);

		tex->r = NULL;
	}
}

static void load_sample(void *buf, int w, int h, int x, int y, vec3_t *s)
{
	unsigned char *p = buf;

	if (x < 0) x = w + x % w;
	if (y < 0) y = h + y % h;

	x = x % w;
	y = y % h;

	p = &p[((h - 1 - y) * w + x) * 3];

	s->x = p[0] / 255.f;
	s->y = p[1] / 255.f;
	s->z = p[2] / 255.f;
}

__attribute__((unused))
static void nearest(void *buf, int w, int h, const vec2_t *uv, vec3_t *s)
{
	load_sample(buf, w, h, uv->x * (w - 1), uv->y * (h - 1), s);
}

__attribute__((unused))
static void bilerp(void *buf, int w, int h, const vec2_t *uv, vec3_t *s)
{
	real_t	fx	= uv->x * (w - 1);
	real_t	fy	= uv->y * (h - 1);
	int	x	= fx;
	int	y	= fy;
	vec3_t	s0[4];
	vec3_t	s1[2];

	fx = fx - x;
	fy = fy - y;

	load_sample(buf, w, h, x + 0, y + 0, &s0[0]);
	load_sample(buf, w, h, x + 1, y + 0, &s0[1]);
	load_sample(buf, w, h, x + 0, y + 1, &s0[2]);
	load_sample(buf, w, h, x + 1, y + 1, &s0[3]);

	vec3_sub(&s1[0], &s0[1], &s0[0]);
	vec3_fma(&s1[0], &s0[0], fx, &s1[0]);

	vec3_sub(&s1[1], &s0[3], &s0[2]);
	vec3_fma(&s1[1], &s0[2], fx, &s1[1]);

	vec3_sub(s, &s1[1], &s1[0]);
	vec3_fma(s, &s1[0], fy, s);
}

vec3_t tex_sample_buf(const tex_t *tex, const vec2_t *uv, void *buf)
{
	vec3_t c;

#ifdef BILERP
		bilerp(buf, tex->w, tex->h, uv, &c);
#else
		nearest(buf, tex->w, tex->h, uv, &c);
#endif

	return c;
}

void tex_sample(const tex_t *tex, const vec2_t *uv,
		vec3_t *c, vec3_t *n, real_t *r)
{
	if (c != NULL && tex->c != NULL)
	{
#ifdef BILERP
		bilerp(tex->c, tex->w, tex->h, uv, c);
#else
		nearest(tex->c, tex->w, tex->h, uv, c);
#endif
	}

	if (n != NULL && tex->n != NULL)
	{
#ifdef BILERP
		bilerp(tex->n, tex->w, tex->h, uv, n);

		n->x = n->x * 2 - 1;
		n->y = n->y * 2 - 1;
		n->z = n->z * 2 - 1;

		vec3_norm(n, n);
#else
		nearest(tex->n, tex->w, tex->h, uv, n);

		n->x = n->x * 2 - 1;
		n->y = n->y * 2 - 1;
		n->z = n->z * 2 - 1;
#endif
	}

	if (r != NULL && tex->r != NULL)
	{
		vec3_t tr;

#ifdef BILERP
		bilerp(tex->r, tex->w, tex->h, uv, &tr);
#else
		nearest(tex->r, tex->w, tex->h, uv, &tr);
#endif

		*r = tr.x;
	}
}
