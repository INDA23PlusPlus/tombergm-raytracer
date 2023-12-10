#ifndef SPH_H
#define SPH_H

#include "mat.h"
#include "ray.h"
#include "vec.h"

typedef struct
{
	vec3_t		c;
	real_t		r;

	const mat_t *	mat;
} sph_t;

int	sph_trace(sph_t *sph, ray_t *ray);
int	ray_sph_light(sph_t *sph, vec3_t *c, vec3_t *p, vec3_t *d,
			real_t *dist, int depth);

#endif
