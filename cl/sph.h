#ifndef SPH_H
#define SPH_H

#include "vec.h"

typedef struct mat_struct	mat_t;
typedef struct ray_struct	ray_t;
typedef struct sph_struct	sph_t;

struct sph_struct
{
	vec3_t		c;
	real_t		r;

	const mat_t *	mat;
};

static inline
void	sph_trace(sph_t *sph, ray_t *ray);
static inline
void	ray_sph_light(	sph_t *sph, vec3_t *c, vec3_t *p,
			vec3_t *d, real_t *dist);

#endif
