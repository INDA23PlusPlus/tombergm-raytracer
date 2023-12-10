#include <math.h>
#include "tri.h"
#include "ray.h"
#include "vec.h"

void tri_precomp(tri_t *tri)
{
	vec_t	n;
	vec_t	i;
	vec_t	j;
	float	s[3];
	float	t[3];
	float	d;

	{
		vec_t	a;
		vec_t	b;

		vec_sub(&a, &tri->a, &tri->c);
		vec_sub(&b, &tri->b, &tri->c);

		vec_cross(&n, &a, &b);
		vec_norm(&n, &n);
	}

	vec_sub(&i, &tri->a, &tri->c);
	vec_norm(&i, &i);
	vec_cross(&j, &i, &n);

	s[0] = vec_dot(&tri->a, &i);
	s[1] = vec_dot(&tri->b, &i);
	s[2] = vec_dot(&tri->c, &i);
	t[0] = vec_dot(&tri->a, &j);
	t[1] = vec_dot(&tri->b, &j);
	t[2] = vec_dot(&tri->c, &j);
	d = (t[1] - t[2]) * (s[0] - s[2]) + (s[2] - s[1]) * (t[0] - t[2]);

	tri->n = n;
	tri->i = i;
	tri->j = j;
	tri->s[0] = s[0];
	tri->s[1] = s[1];
	tri->s[2] = s[2];
	tri->t[0] = t[0];
	tri->t[1] = t[1];
	tri->t[2] = t[2];
	tri->d = d;
}

static void uvmap(tri_t *tri, vec_t *q, vec2_t *uv)
{
	vec2_t	at	= tri->at;
	vec2_t	bt	= tri->bt;
	vec2_t	ct	= tri->ct;
	vec2_t	iv;
	vec2_t	jv;
	vec_t	iw;
	vec_t	jw;
	vec_t	p;
	vec2_t	st;

	vec2_sub(&iv, &bt, &at);
	vec2_sub(&jv, &ct, &at);

	vec_sub(&iw, &tri->b, &tri->a);
	vec_sub(&jw, &tri->c, &tri->a);

	vec_sub(&p, q, &tri->a);

	{
		float ix;
		float iy;
		float jx;
		float jy;
		float px;
		float py;

		float dxy = fabsf(iw.x * jw.y - iw.y * jw.x);
		float dyz = fabsf(iw.y * jw.z - iw.z * jw.y);
		float dzx = fabsf(iw.z * jw.x - iw.x * jw.z);

		if (dxy >= dyz && dyz >= dzx)
		{
			ix = iw.x;
			iy = iw.y;
			jx = jw.x;
			jy = jw.y;
			px = p.x;
			py = p.y;
		}
		else if (dyz >= dzx && dzx >= dxy)
		{
			ix = iw.y;
			iy = iw.z;
			jx = jw.y;
			jy = jw.z;
			px = p.y;
			py = p.z;
		}
		else /* if (dzx >= dxy && dxy >= dyz) */
		{
			ix = iw.z;
			iy = iw.x;
			jx = jw.z;
			jy = jw.x;
			px = p.z;
			py = p.x;
		}

		st.x = (px * jy - jx * py) / (ix * jy - jx * iy);
		st.y = (ix * py - px * iy) / (ix * jy - jx * iy);
	}

	uv->x = at.x + iv.x * st.x + jv.x * st.y;
	uv->y = at.y + iv.y * st.x + jv.y * st.y;
}

int tri_trace(tri_t* tri, ray_t *ray)
{
	vec_t	q;
	float	s;
	float	t;

	if (ray->prev == tri)
	{
		return 0;
	}

	t = vec_dot(&ray->d, &tri->n);

#if 0
	if (t >= 0)
	{
		/* Surface is facing away */
		return 0;
	}
#endif

	s = vec_dot(&tri->a, &tri->n);
	t = (s - vec_dot(&ray->p, &tri->n)) / t;

	if (t <= 0)
	{
		/* Surface is behind ray origin */
		return 0;
	}
	else if (t >= ray->l)
	{
		/* Surface is occluded */
		return 0;
	}

	vec_fma(&q, &ray->p, t, &ray->d);

	float	x	= vec_dot(&q, &tri->i);
	float	y	= vec_dot(&q, &tri->j);

	float	l_1	=
	(
		(tri->t[1] - tri->t[2]) * (x - tri->s[2]) +
		(tri->s[2] - tri->s[1]) * (y - tri->t[2])
	) / tri->d;
	float	l_2	=
	(
		(tri->t[2] - tri->t[0]) * (x - tri->s[2]) +
		(tri->s[0] - tri->s[2]) * (y - tri->t[2])
	) / tri->d;
	float	l_3	= 1 - l_1 - l_2;

	if (	(l_1 >= 0 && l_1 <= 1)	&&
		(l_2 >= 0 && l_2 <= 1)	&&
		(l_3 >= 0 && l_3 <= 1)	)
	{
		ray->curr = tri;

		vec_set(&ray->q, &q);
		vec_set(&ray->n, &tri->n);
		ray->l = t;

		ray->mat = tri->mat;
		uvmap(tri, &ray->q, &ray->uv);

		return 1;
	}

	return 0;
}
