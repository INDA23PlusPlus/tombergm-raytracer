#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cam.h"
#include "render.h"
#include "rt.h"
#include "scene.h"
#include "vp.h"

#ifdef DEBUG
#define N_THRD	0
#else
#define N_THRD	4
#endif
#define WIDTH	800
#define HEIGHT	600

#if N_THRD != 0
static render_task_t	rt_list[N_THRD];
#endif

static GLuint		fbo;
static GLuint		tex;
static unsigned char	buf[WIDTH * HEIGHT * 3];

static float		r;
static float		t;
static cam_t		cam;
static vp_t		vp;
static char		keys[256];

int			mouse_x;
int			mouse_y;
int			mouse_l;
int			mouse_r;
int			render_x;
int			render_y;

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

	t = t + 0.01;
}

static void display_func(void)
{
	update();

#if N_THRD != 0
	render_task_commit(rt_list, N_THRD, &cam, &vp, buf);
	render_task_wait(rt_list, N_THRD);
#else
	render(&cam, &vp, buf);
#endif

	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGB,
				WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE,
				buf);
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
	float	aspect	= (float) WIDTH / (float) HEIGHT;

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
	vp.ulx		= -aspect;
	vp.uly		= -1.;

	scene_init();

#if N_THRD != 0
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

#if N_THRD != 0
	render_task_dstr(rt_list, N_THRD);
#endif

	scene_dstr();
}
