#ifndef TRI_H
#define TRI_H

#include "mat.h"
#include "ray.h"
#include "vec.h"

typedef struct
{
	vec_t		a;
	vec_t		b;
	vec_t		c;

	const mat_t *	mat;

	vec2_t		at;
	vec2_t		bt;
	vec2_t		ct;

	/* Precompute */
	struct
	{
		vec_t	n;
		vec_t	i;
		vec_t	j;
		float	s[3];
		float	t[3];
		float	d;
	};
} tri_t;

void	tri_precomp(tri_t *tri);
int	tri_trace(tri_t* tri, ray_t *ray);

#endif
