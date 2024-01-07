#include <stdio.h>
#include <tgmath.h>
#include <fcntl.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cam.h"
#include "clrender.h"
#include "render.h"
#include "rt.h"
#include "scene.h"
#include "vec.h"
#include "vp.h"

#define CONFIG_MT
#define CONFIG_CL

#define N_THRD	4

#define WIDTH	1280
#define HEIGHT	720

extern scene_t		scene;

static rt_if_t		rt_if;
static rt_if_t		rt_if_prev;

#ifdef CONFIG_MT
static render_task_t	rt_list[N_THRD];
#endif

static GLuint		fbo;
static GLuint		tex;
static unsigned char	pb[WIDTH * HEIGHT * 4];
static vec3_t		sb[WIDTH * HEIGHT];
static int		sn;

static real_t		r;
static real_t		p;
static cam_t		cam;
static vp_t		vp;
static char		keys[256];

int			mouse_x;
int			mouse_y;
int			mouse_l;
int			mouse_r;
int			render_x;
int			render_y;

static int export(void)
{
	int ret	= 0;
#ifdef O_BINARY
	int m	= O_WRONLY | O_BINARY | O_CREAT | O_TRUNC;
#else
	int m	= O_WRONLY | O_CREAT | O_TRUNC;
#endif
	int s	= sizeof(pb);
	int fd	= open("./rt.bmp", m, 0664);

#define wr(t, d) \
({ \
	t wr_data__ = (d); \
	int wr_ret__ = write(fd, &wr_data__, sizeof(wr_data__)); \
	if (wr_ret__ != sizeof(wr_data__)) \
	{ \
		ret = -1; \
		goto exit; \
	} \
})
	wr(	short,	0x4D42	);
	wr(	int,	54 + s	);
	wr(	short,	0	);
	wr(	short,	0	);
	wr(	int,	54	);

	wr(	int,	40	);
	wr(	int,	WIDTH	);
	wr(	int,	HEIGHT	);
	wr(	short,	1	);
	wr(	short,	32	);
	wr(	int,	0	);
	wr(	int,	s	);
	wr(	int,	0	);
	wr(	int,	0	);
	wr(	int,	0	);
	wr(	int,	0	);
#undef wr

	if (write(fd, pb, s) != s)
	{
		ret = -1;
		goto exit;
	}

	fprintf(stderr, "Image saved\n");

exit:
	if (fd != -1)
	{
		close(fd);
	}

	return ret;
}

static void rt_finish(void)
{
	switch (rt_if_prev)
	{
#ifdef CONFIG_MT
		case RT_MT:
		{
			render_task_wait(rt_list, N_THRD);
		}	break;
#endif

#ifdef CONFIG_CL
		case RT_CL:
		{
			clrender_wait();
		}	break;
#endif

		default:
			break;
	}

	rt_if_prev = RT_NULL;
}

static void rt_commit(void)
{
	switch (rt_if)
	{
		case RT_ST:
		{
			render(&scene, &cam, &vp, pb, sb, sn);
		}	break;

#ifdef CONFIG_MT
		case RT_MT:
		{
			render_task_commit(	rt_list, N_THRD, &scene,
						&cam, &vp, pb, sb, sn);
		}	break;
#endif

#ifdef CONFIG_CL
		case RT_CL:
		{
			clrender_commit(&scene, &cam, &vp, pb, sb, sn);
		}	break;
#endif

		default:
			break;
	}

	rt_if_prev = rt_if;
}

static void update(void)
{
	if (keys[GLUT_KEY_LEFT])
	{
		if (keys[GLUT_KEY_SHIFT_L] || keys[GLUT_KEY_SHIFT_R])
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
		if (keys[GLUT_KEY_SHIFT_L] || keys[GLUT_KEY_SHIFT_R])
		{
			cam.p.x = cam.p.x + cam.fv.z * 0.1;
			cam.p.z = cam.p.z - cam.fv.x * 0.1;
		}
		else
		{
			r = r + 0.1;
		}
	}

	if (keys['Z'])
	{
		p = p + 0.1;
	}

	if (keys['X'])
	{
		p = p - 0.1;
	}

	cam.fv.x = sin(r) * cos(p);
	cam.fv.y = sin(p);
	cam.fv.z = cos(r) * cos(p);

	cam.uv.x = sin(r) * -sin(p);
	cam.uv.y = cos(p);
	cam.uv.z = cos(r) * -sin(p);

	if (keys[GLUT_KEY_UP])
	{
		vec3_fma(&cam.p, &cam.p, 0.1, &cam.fv);
	}

	if (keys[GLUT_KEY_DOWN])
	{
		vec3_fma(&cam.p, &cam.p, -0.1, &cam.fv);
	}

	if (keys[' '])
	{
		cam.p.y = cam.p.y + 0.1;
	}

	if (keys[GLUT_KEY_CTRL_L] || keys[GLUT_KEY_CTRL_R])
	{
		cam.p.y = cam.p.y - 0.1;
	}
}

static void display_func(void)
{
	static int		time_set;
	static struct timespec	disp_time;
	static struct timespec	rate_time;
	static int		rate_frame;
	static long		rate_ms;
	static float		fps;

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	if (time_set)
	{
		long dd	= 	(now.tv_sec - disp_time.tv_sec) * 1000000l
				+ (now.tv_nsec - disp_time.tv_nsec) / 1000l;
		long rd	= 	(now.tv_sec - rate_time.tv_sec) * 1000000l
				+ (now.tv_nsec - rate_time.tv_nsec) / 1000l;

		disp_time = now;

		rate_frame++;
		rate_ms = rate_ms + dd;

		if (rd >= 1000000l)
		{
			fps = rate_frame * (1000000.f / rate_ms);

			rate_time = now;
			rate_frame = 0;
			rate_ms = 0;
		}
	}
	else
	{
		disp_time = now;
		rate_time = now;

		time_set = 1;
	}

	rt_finish();

	update();

	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGB,
				WIDTH, HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE,
				pb);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(	GL_READ_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_2D, tex, 0);
		glBlitFramebuffer(	0, 0, WIDTH, HEIGHT,
					0, 0, WIDTH, HEIGHT,
					GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		glutSwapBuffers();
		glutPostRedisplay();
	}

	rt_commit();

	{
		char t[64];

		if (sn == 0)
		{
			sprintf(t, "FPS: % 6.2f    Samples: % 6i", fps, 1);
		}
		else
		{
			sprintf(t, "FPS: % 6.2f    Samples: % 6i", fps, sn);
		}

		glutSetWindowTitle(t);

		fprintf(stderr, "%s\r", t);
	}

	if (sn != 0)
	{
		sn++;
	}
}

static void motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = HEIGHT - 1 - y;
}

static void mouse_func(int bn, int st, int x, int y)
{
	if (bn == GLUT_LEFT_BUTTON)
	{
		mouse_l = (st == GLUT_DOWN);
	}
	else if (bn == GLUT_RIGHT_BUTTON)
	{
		mouse_r = (st == GLUT_DOWN);
	}
}

static void keyboard_func(unsigned char key, int x, int y)
{
	if (key >= 'a' && key <= 'z')
	{
		key = key + ('A' - 'a');
	}

	keys[key] = 1;

	if (key == 'S')
	{
		export();
	}
	else if (key == 13)
	{
		if (sn == 0)
		{
			sn = 1;
		}
		else
		{
			sn = 0;
		}
	}
	else if (key == 27)
	{
		glutLeaveMainLoop();
	}
}

static void keyboard_up_func(unsigned char key, int x, int y)
{
	if (key >= 'a' && key <= 'z')
	{
		key = key + ('A' - 'a');
	}

	keys[key] = 0;
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
	real_t	aspect	= (real_t) WIDTH / (real_t) HEIGHT;

	cam.p.x		= 0;
	cam.p.y		= 0;
	cam.p.z 	= -1;
	cam.uv.x	= 0;
	cam.uv.y	= 1;
	cam.uv.z	= 0;
	cam.fv.x	= 0;
	cam.fv.y	= 0;
	cam.fv.z	= 1;
	cam.l		= -aspect;
	cam.r		= aspect;
	cam.t		= 1;
	cam.b		= -1;
	cam.n		= 1;

	vp.w		= WIDTH;
	vp.h		= HEIGHT;

	scene_init(&scene);

#ifdef CONFIG_CL
	if (clrender_init(&scene, pb, &vp) == 0)
	{
		rt_if = RT_CL;
	}
	else
	{
		fprintf(stderr, "OpenCL unavailable\n");
	}
#endif

#ifdef CONFIG_MT
	render_task_init(rt_list, N_THRD);

	if (rt_if == RT_NULL)
	{
		fprintf(stderr,
			"Using CPU rendering with %i threads\n",
			N_THRD);

		rt_if = RT_MT;
	}
#endif

	if (rt_if == RT_NULL)
	{
		fprintf(stderr,
			"Using single-threaded CPU rendering\n");

		rt_if = RT_ST;
	}

	if (rt_if != RT_ST)
	{
		rt_commit();
	}

	{
		glutInit(&argc, argv);

		glutSetOption(	GLUT_ACTION_ON_WINDOW_CLOSE,
				GLUT_ACTION_CONTINUE_EXECUTION);

		glutInitWindowSize(WIDTH, HEIGHT);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
		glutCreateWindow("");

		glewInit();

		glGenTextures(1, &tex);
		glGenFramebuffers(1, &fbo);

		glutDisplayFunc(display_func);
		glutMotionFunc(motion_func);
		glutPassiveMotionFunc(motion_func);
		glutMouseFunc(mouse_func);
		glutKeyboardFunc(keyboard_func);
		glutKeyboardUpFunc(keyboard_up_func);
		glutSpecialFunc(special_func);
		glutSpecialUpFunc(special_up_func);

		glutMainLoop();
	}

	rt_finish();

#ifdef CONFIG_MT
	render_task_dstr(rt_list, N_THRD);
#endif

	scene_dstr(&scene);
}
