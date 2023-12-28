#ifndef SPH_H
#define SPH_H

#include "box.h"
#include "mat.h"
#include "ray.h"
#include "vec.h"

typedef struct
{
	vec3_t		c;
	real_t		r;

	const mat_t *	mat;
} sph_t;

void	sph_get_box(const sph_t *sph, box_t *box);
real_t	sph_trace(	const sph_t *sph, vec3_t *p, vec3_t *d,
			real_t m, void *prev);
void	sph_hit(const sph_t *sph, ray_t *ray);

#endif
