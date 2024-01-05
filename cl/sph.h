#ifndef SPH_H
#define SPH_H

#include "scene.h"
#include "vec.h"

typedef __constant struct mat_struct	mat_t;
typedef struct ray_struct		ray_t;
typedef __constant struct sph_struct	sph_t;

struct sph_struct
{
	vec3_t		c;
	real_t		r;

	int		mat;

	char		_pad0[8];
};

real_t	sph_trace(const sph_t *sph, vec3_t *p, vec3_t *d, real_t m, bool prev);
void	sph_hit(SCENE, const sph_t *sph, ray_t *ray);

#endif
