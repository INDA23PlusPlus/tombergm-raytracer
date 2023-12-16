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

#define RT_CL

#define WIDTH	800
#define HEIGHT	600

#ifndef DEBUG
#define N_THRD	4
#else
#define N_THRD	0
#endif

#ifdef RT_CL
#elif N_THRD != 0
static render_task_t	rt_list[N_THRD];
#endif

static GLuint		fbo;
static GLuint		tex;
static unsigned char	pb[WIDTH * HEIGHT * 4];
static vec3_t		sb[WIDTH * HEIGHT];
static int		sn;

static real_t		r;
static real_t		t;
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

static void update(void)
{
	if (keys[GLUT_KEY_LEFT])
	{
		if (keys[GLUT_KEY_CTRL_L] || keys[GLUT_KEY_CTRL_R])
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
		if (keys[GLUT_KEY_CTRL_L] || keys[GLUT_KEY_CTRL_R])
		{
			cam.p.x = cam.p.x + cam.fv.z * 0.1;
			cam.p.z = cam.p.z - cam.fv.x * 0.1;
		}
		else
		{
			r = r + 0.1;
		}
	}

	cam.fv.z = cos(r);
	cam.fv.x = sin(r);

	if (keys[GLUT_KEY_UP])
	{
		vec3_fma(&cam.p, &cam.p, 0.1, &cam.fv);
	}

	if (keys[GLUT_KEY_DOWN])
	{
		vec3_fma(&cam.p, &cam.p, -0.1, &cam.fv);
	}

	t = t + 0.01;
}

static void display_func(void)
{
	struct timespec	time_a;
	struct timespec	time_b;

	update();

	clock_gettime(CLOCK_REALTIME, &time_a);
#ifdef RT_CL
	clrender_commit(&cam, &vp, pb, sb, sn);
	clrender_wait();
#elif N_THRD != 0
	render_task_commit(rt_list, N_THRD, &cam, &vp, pb, sb, sn);
	render_task_wait(rt_list, N_THRD);
#else
	render(&cam, &vp, pb, sb, sn);
#endif
	clock_gettime(CLOCK_REALTIME, &time_b);

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

	{
		int s = time_b.tv_sec - time_a.tv_sec;
		int n = time_b.tv_nsec - time_a.tv_nsec;

		if (n < 0)
		{
			s = s - 1;
			n = n + 1000000000;
		}

		fprintf(stderr, "Frame time: %i.%09i\n", s, n);
	}

	if (sn != 0)
	{
		fprintf(stderr, "Samples: %i\n", sn++);
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
	if (key == 'S' || key == 's')
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
	cam.p.z 	= 0;
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

	scene_init();

#ifdef RT_CL
	clrender_init(pb, &vp, &scene);
#elif N_THRD != 0
	render_task_init(rt_list, N_THRD);
#endif

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
		glutSpecialFunc(special_func);
		glutSpecialUpFunc(special_up_func);

		glutMainLoop();
	}

#ifdef RT_CL
#elif N_THRD != 0
	render_task_dstr(rt_list, N_THRD);
#endif

	scene_dstr();
}
