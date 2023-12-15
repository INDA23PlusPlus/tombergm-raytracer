#include <stddef.h>
#include <tgmath.h>
#include <pthread.h>
#include "cam.h"
#include "ray.h"
#include "render.h"
#include "rt.h"
#include "vp.h"
#include "vec.h"

#define TASK_NONE	-1
#define TASK_CANCEL	-2

static int rt_work(	int task_num, int task_max,
			cam_t *cam, vp_t *vp,
			unsigned char *pb, vec3_t *sb, int sn)
{
	vec3_t	p	= cam->p;
	vec3_t	rv	= { 0, 0, 0 };
	vec3_t	uv	= cam->uv;
	vec3_t	fv	= cam->fv;
	real_t	l	= cam->l;
	real_t	r	= cam->r;
	real_t	t	= cam->t;
	real_t	b	= cam->b;
	real_t	n	= cam->n;
	int	w	= vp->w;
	int	h	= vp->h;

	vec3_cross(&rv, &uv, &fv);

	for (int y = task_num; y < h; y = y + task_max)
	{
		for (int x = 0; x < w; x++)
		{
			unsigned char *	pix	= &pb[(y * w + x) * 4];
			vec3_t		c;
			vec3_t		d;
			real_t		rs	= l + x * (r - l) / (w - 1);
			real_t		us	= b + y * (t - b) / (h - 1);
			real_t		fs	= n;

#ifdef DEBUG
			render_x = x;
			render_y = y;
#endif

			vec3_set(&c, &vec3_zero);

			rs = rs + (r - l) / (w - 1) * (0.5 - flt_rand());
			us = us + (t - b) / (h - 1) * (0.5 - flt_rand());

			vec3_scale(&d, rs, &rv);
			vec3_fma(&d, &d, us, &uv);
			vec3_fma(&d, &d, fs, &fv);
			vec3_norm(&d, &d);

			ray_trace(&c, &p, &d, NULL);

			if (sn != 0)
			{
				vec3_t *s = &sb[y * w + x];

				if (sn == 1)
				{
					*s = c;
				}
				else
				{
					real_t m = sn;

#if 1
					vec3_add(s, s, &c);
					vec3_scale(&c, 1 / m, s);
#else
					vec3_scale(s, (m - 1) / m, s);
					vec3_fma(s, s, 1 / m, &c);

					c = *s;
#endif
				}
			}

#if 1
			/* Gamma correction */
			c.x = pow(c.x, 1. / 2.2);
			c.y = pow(c.y, 1. / 2.2);
			c.z = pow(c.z, 1. / 2.2);
#endif

			vec3_clamp(&c, &c, &vec3_zero, &vec3_unit);

			pix[0] = 0.5 + c.z * 255;
			pix[1] = 0.5 + c.y * 255;
			pix[2] = 0.5 + c.x * 255;
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
			rt->cam, rt->vp, rt->pb, rt->sb, rt->sn);

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
			cam_t *cam, vp_t *vp,
			unsigned char *pb, vec3_t *sb, int sn)
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
			rt->pb = pb;
			rt->sb = sb;
			rt->sn = sn;

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

int render(cam_t *cam, vp_t *vp, unsigned char *pb, vec3_t *sb, int sn)
{
	return rt_work(0, 1, cam, vp, pb, sb, sn);
}
