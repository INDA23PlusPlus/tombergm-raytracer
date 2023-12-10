#ifndef SPH_H
#define SPH_H

#include "mat.h"
#include "ray.h"
#include "vec.h"

typedef struct
{
	vec_t		c;
	float		r;

	const mat_t *	mat;
} sph_t;

int	sph_trace(sph_t *sph, ray_t *ray);
int	ray_sph_light(sph_t *sph, vec_t *c, vec_t *p, vec_t *d,
			float *dist, int depth);

#endif
