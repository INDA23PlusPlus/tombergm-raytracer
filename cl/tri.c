#include "tri.h"
#include "ray.h"
#include "vec.h"

static void uvmap(tri_t *tri, vec3_t *q, vec2_t *uv)
{
	vec3_t	p;
	vec3_t	i;
	vec3_t	j;
	vec2_t	st;

	p = *q - tri->a;

	i = cross(p, tri->iw);
	j = cross(p, tri->jw);

	st.x = sqrt(dot(j, j) * tri->td);
	st.y = sqrt(dot(i, i) * tri->td);

	*uv = tri->at + st.x * tri->iv;
	*uv = *uv + st.y * tri->jv;
}

void tri_trace(tri_t *tri, ray_t *ray)
{
	vec3_t	q;
	real_t	d;
	real_t	l;

	if (ray->prev == tri)
	{
		return;
	}

	d = dot(tri->a, tri->n);
	l = (d - dot(ray->p, tri->n)) / dot(ray->d, tri->n);

	if (l <= 0)
	{
		/* Surface is behind ray origin */
		return;
	}
	else if (l >= ray->l)
	{
		/* Surface is occluded */
		return;
	}

	q = ray->p + l * ray->d;

	{
		real_t	x	= dot(q, tri->i);
		real_t	y	= dot(q, tri->j);
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
			return;
		}
	}

	{
		ray->curr = tri;

		ray->q = q;
		ray->n = tri->n;
		ray->l = l;

		ray->mat = tri->mat;
		if (mat_has_tex(ray->mat))
		{
			uvmap(tri, &ray->q, &ray->uv);

			ray->tu = tri->tu;
			ray->tv = tri->tv;
		}
	}
}

real_t tri_trace2(	const tri_t *tri, vec3_t *p, vec3_t *d,
			real_t m, void *prev)
{
	vec3_t	q;
	real_t	l;

	if (tri == prev)
	{
		return INFINITY;
	}

	l = dot(tri->a, tri->n) - dot(*p, tri->n);
	l = l / dot(*d, tri->n);

	if (0 >= l || l >= m)
	{
		return INFINITY;
	}

	q = *p + l * *d;

	{
		real_t	x	= dot(q, tri->i);
		real_t	y	= dot(q, tri->j);
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
			return INFINITY;
		}
	}

	return l;
}

static void tri_map(const tri_t *tri, vec3_t *q, vec2_t *uv)
{
	vec3_t	p;
	vec3_t	i;
	vec3_t	j;
	vec2_t	st;

	p = *q - tri->a;

	i = cross(p, tri->iw);
	j = cross(p, tri->jw);

	st.x = sqrt(dot(j, j) * tri->td);
	st.y = sqrt(dot(i, i) * tri->td);

	*uv = tri->at + st.x * tri->iv + st.y * tri->jv;
}

void tri_hit(const tri_t *tri, ray_t *ray)
{
	ray->n = tri->n;

	ray->mat = tri->mat;

	if (mat_has_tex(ray->mat))
	{
		tri_map(tri, &ray->q, &ray->uv);

		ray->tu = tri->tu;
		ray->tv = tri->tv;
	}
}
