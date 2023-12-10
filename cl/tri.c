#include "tri.h"
#include "ray.h"
#include "vec.h"

static inline void uvmap(tri_t *tri, vec3_t *q, vec2_t *uv)
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

static inline void tri_trace(tri_t *tri, ray_t *ray)
{
	vec3_t	q;
	real_t	s;
	real_t	t;

	if (ray->prev == tri)
	{
		return;
	}

	s = dot(tri->a, tri->n);
	t = (s - dot(ray->p, tri->n)) / dot(ray->d, tri->n);

	if (t <= 0)
	{
		/* Surface is behind ray origin */
		return;
	}
	else if (t >= ray->l)
	{
		/* Surface is occluded */
		return;
	}

	q = ray->p + t * ray->d;

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
		ray->l = t;

		ray->mat = tri->mat;
		if (mat_has_tex(ray->mat))
		{
			uvmap(tri, &ray->q, &ray->uv);

			ray->tu = tri->tu;
			ray->tv = tri->tv;
		}
	}
}
