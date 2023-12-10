#include <tgmath.h>
#include "tri.h"
#include "ray.h"
#include "vec.h"

void tri_precomp(tri_t *tri)
{
	vec3_t	n;
	vec3_t	i;
	vec3_t	j;
	vec2_t	pa;
	vec2_t	pb;
	vec2_t	pc;
	real_t	d;
	vec2_t	iv;
	vec2_t	jv;
	vec3_t	iw;
	vec3_t	jw;
	real_t	td;
	vec3_t	tu;
	vec3_t	tv;

	{
		vec3_t	a;
		vec3_t	b;

		vec3_sub(&a, &tri->a, &tri->c);
		vec3_sub(&b, &tri->b, &tri->c);

		vec3_cross(&n, &a, &b);
		vec3_norm(&n, &n);
	}

	vec3_sub(&i, &tri->a, &tri->c);
	vec3_norm(&i, &i);
	vec3_cross(&j, &i, &n);

	pa.x = vec3_dot(&tri->a, &i);
	pa.y = vec3_dot(&tri->a, &j);
	pb.x = vec3_dot(&tri->b, &i);
	pb.y = vec3_dot(&tri->b, &j);
	pc.x = vec3_dot(&tri->c, &i);
	pc.y = vec3_dot(&tri->c, &j);
	d = 1 / (
			+ (pa.x - pc.x) * (pb.y - pc.y)
			- (pb.x - pc.x) * (pa.y - pc.y)
		);

	vec2_sub(&iv, &tri->bt, &tri->at);
	vec2_sub(&jv, &tri->ct, &tri->at);
	vec3_sub(&iw, &tri->b, &tri->a);
	vec3_sub(&jw, &tri->c, &tri->a);

	{
		vec3_t	v;

		vec3_cross(&v, &iw, &jw);

		td = 1 / vec3_len_sq(&v);
	}

	{
		vec2_t u;
		vec2_t v;

		u.x =  jv.y / (iv.x * jv.y - iv.y * jv.x);
		u.y = -iv.y / (iv.x * jv.y - iv.y * jv.x);

		v.x = -jv.x / (iv.x * jv.y - iv.y * jv.x);
		v.y =  iv.x / (iv.x * jv.y - iv.y * jv.x);

		tu.x = iw.x * u.x + jw.x * u.y;
		tu.y = iw.y * u.x + jw.y * u.y;
		tu.z = iw.z * u.x + jw.z * u.y;
		vec3_norm(&tu, &tu);

		tv.x = iw.x * v.x + jw.x * v.y;
		tv.y = iw.y * v.x + jw.y * v.y;
		tv.z = iw.z * v.x + jw.z * v.y;
		vec3_norm(&tv, &tv);
	}

	tri->n	= n;
	tri->i	= i;
	tri->j	= j;
	tri->pa	= pa;
	tri->pb	= pb;
	tri->pc	= pc;
	tri->d	= d;
	tri->iv	= iv;
	tri->jv	= jv;
	tri->iw	= iw;
	tri->jw	= jw;
	tri->td	= td;
	tri->tu	= tu;
	tri->tv	= tv;
}

static void uvmap(tri_t *tri, vec3_t *q, vec2_t *uv)
{
	vec3_t	p;
	vec3_t	i;
	vec3_t	j;
	vec2_t	st;

	vec3_sub(&p, q, &tri->a);

	vec3_cross(&i, &p, &tri->iw);
	vec3_cross(&j, &p, &tri->jw);

	st.x = sqrt(vec3_len_sq(&j) * tri->td);
	st.y = sqrt(vec3_len_sq(&i) * tri->td);

	vec2_fma(uv, &tri->at, st.x, &tri->iv);
	vec2_fma(uv, uv, st.y, &tri->jv);
}

int tri_trace(tri_t* tri, ray_t *ray)
{
	vec3_t	q;
	real_t	s;
	real_t	t;

	if (ray->prev == tri)
	{
		return 0;
	}

	s = vec3_dot(&tri->a, &tri->n);
	t = (s - vec3_dot(&ray->p, &tri->n)) / vec3_dot(&ray->d, &tri->n);

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

	vec3_fma(&q, &ray->p, t, &ray->d);

	{
		real_t	x	= vec3_dot(&q, &tri->i);
		real_t	y	= vec3_dot(&q, &tri->j);
		real_t	l_1	=
		(
			+ (tri->pb.y - tri->pc.y) * (x - tri->pc.x)
			- (tri->pb.x - tri->pc.x) * (y - tri->pc.y)
		) * tri->d;
		real_t	l_2	=
		(
			- (tri->pa.y - tri->pc.y) * (x - tri->pc.x)
			+ (tri->pa.x - tri->pc.x) * (y - tri->pc.y)
		) * tri->d;
		real_t	l_3	= 1 - l_1 - l_2;

		if (l_1 < 0 || l_2 < 0 || l_3 < 0)
		{
			/* Intersection lies outside triangle */
			return 0;
		}
	}

	{
		ray->curr = tri;

		vec3_set(&ray->q, &q);
		vec3_set(&ray->n, &tri->n);
		ray->l = t;

		ray->mat = tri->mat;
		if (mat_has_tex(ray->mat))
		{
			uvmap(tri, &ray->q, &ray->uv);

			ray->tu = tri->tu;
			ray->tv = tri->tv;
		}

		return 1;
	}
}
