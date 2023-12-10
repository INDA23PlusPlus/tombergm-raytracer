#include <math.h>
#include <stddef.h>
#include <pthread.h>
#include "cam.h"
#include "ray.h"
#include "render.h"
#include "rt.h"
#include "vp.h"
#include "vec.h"

#define TASK_NONE	-1
#define TASK_CANCEL	-2

int render(cam_t *cam, vp_t *vp, unsigned char *buf)
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

			render_x = x;
			render_y = y;

			vec_fma(&d, &vec_zero, rs, &rv);
			vec_fma(&d, &d, us, &uv);
			vec_fma(&d, &d, fs, &fv);
			vec_norm(&d, &d);

			vec_set(&c, &vec_zero);

			ray_trace(&c, &p, &d, NULL);

			vec_clamp(&c, &c, &vec_zero, &vec_unit);

			buf[i + 0] = c.x * nextafter(256, 0);
			buf[i + 1] = c.y * nextafter(256, 0);
			buf[i + 2] = c.z * nextafter(256, 0);
		}
	}

	return 0;
}

static int rt_work(	int task_num, int task_max,
			cam_t *cam, vp_t *vp, unsigned char *buf)
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

	for (int y = task_num; y < h; y = y + task_max)
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

			vec_set(&c, &vec_zero);

			ray_trace(&c, &p, &d, NULL);

			vec_clamp(&c, &c, &vec_zero, &vec_unit);

			buf[i + 0] = c.x * nextafter(256, 0);
			buf[i + 1] = c.y * nextafter(256, 0);
			buf[i + 2] = c.z * nextafter(256, 0);
		}
	}

	return 0;
}

static void *rt_func(void *arg)
{
	render_task_t *rt = arg;

	for (;;)
	{
		pthread_mutex_lock(&rt->mtx);
		while (rt->task_num == TASK_NONE)
		{
			pthread_cond_wait(&rt->cnd, &rt->mtx);
		}
		pthread_mutex_unlock(&rt->mtx);

		if (rt->task_num == TASK_CANCEL)
		{
			break;
		}

		rt_work(rt->task_num, rt->task_max,
			rt->cam, rt->vp, rt->buf);

		pthread_mutex_lock(&rt->mtx);
		{
			rt->task_num = TASK_NONE;
			pthread_cond_signal(&rt->cnd);
		}
		pthread_mutex_unlock(&rt->mtx);
	}

	return arg;
}

int render_task_init(render_task_t *rt_list, int rt_num)
{
	for (int i = 0; i < rt_num; i++)
	{
		render_task_t *rt = &rt_list[i];

		pthread_mutex_init(&rt->mtx, NULL);
		pthread_cond_init(&rt->cnd, NULL);

		rt->task_num = TASK_NONE;

		pthread_create(&rt->thrd, NULL, rt_func, rt);
	}

	return 0;
}

void render_task_commit(render_task_t *rt_list, int rt_num,
			cam_t *cam, vp_t *vp, unsigned char *buf)
{
	for (int i = 0; i < rt_num; i++)
	{
		render_task_t *rt = &rt_list[i];

		pthread_mutex_lock(&rt->mtx);
		{
			rt->task_num = i;
			rt->task_max = rt_num;

			rt->cam = cam;
			rt->vp = vp;
			rt->buf = buf;

			pthread_cond_signal(&rt->cnd);
		}
		pthread_mutex_unlock(&rt->mtx);
	}
}

void render_task_wait(render_task_t *rt_list, int rt_num)
{
	for (int i = 0; i < rt_num; i++)
	{
		render_task_t *rt = &rt_list[i];

		pthread_mutex_lock(&rt->mtx);
		while (rt->task_num != TASK_NONE)
		{
			pthread_cond_wait(&rt->cnd, &rt->mtx);
		}
		pthread_mutex_unlock(&rt->mtx);
	}
}

void render_task_dstr(render_task_t *rt_list, int rt_num)
{
	for (int i = 0; i < rt_num; i++)
	{
		render_task_t *rt = &rt_list[i];

		pthread_mutex_lock(&rt->mtx);
		{
			rt->task_num = TASK_CANCEL;

			pthread_cond_signal(&rt->cnd);
		}
		pthread_mutex_unlock(&rt->mtx);

		pthread_join(rt->thrd, NULL);

		pthread_mutex_destroy(&rt->mtx);
		pthread_cond_destroy(&rt->cnd);
	}
}
