#ifndef SPH_H
#define SPH_H

#include "vec.h"

typedef struct box_struct	box_t;
typedef struct mat_struct	mat_t;
typedef struct ray_struct	ray_t;
typedef struct sph_struct	sph_t;

struct sph_struct
{
	vec3_t		c;
	real_t		r;

	const mat_t *	mat;
};

void	sph_get_box(const sph_t *sph, box_t *box);
real_t	sph_trace(	const sph_t *sph, vec3_t *p, vec3_t *d,
			real_t m, void *prev);
void	sph_hit(const sph_t *sph, ray_t *ray);

#endif
