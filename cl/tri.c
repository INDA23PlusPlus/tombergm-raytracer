#include "mat.h"
#include "scene.h"
#include "tri.h"
#include "ray.h"
#include "vec.h"

real_t tri_trace(const tri_t *tri, ray_t *ray, bool prev)
{
	vec3_t	q;
	real_t	l;

	if (prev)
	{
		return INFINITY;
	}

	l = dot(tri->a, tri->n) - dot(ray->p, tri->n);
	l = l / dot(ray->d, tri->n);

	if (0 >= l || l >= ray->l)
	{
		return INFINITY;
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
			return INFINITY;
		}

		ray->n = l_1 * tri->an + l_2 * tri->bn + l_3 * tri->cn;
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

void tri_hit(SCENE, const tri_t *tri, ray_t *ray)
{
	ray->mat = tri->mat;

	if (mat_has_tex(MAT(ray->mat)))
	{
		tri_map(tri, &ray->q, &ray->uv);

		ray->tu = tri->tu;
		ray->tv = tri->tv;
	}
}
