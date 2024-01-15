#ifndef PRIM_H
#define PRIM_H

#include "scene.h"
#include "vec.h"

#define PRIM_NULL	-1
#define PRIM_TRI	0
#define PRIM_SPH	1

typedef __constant struct prim_struct	prim_t;
typedef struct ray_struct		ray_t;
typedef __constant struct sph_struct	sph_t;
typedef __constant struct tri_struct	tri_t;

struct prim_struct
{
	int	type;
	int	idx;
};

real_t	prim_trace(SCENE, const prim_t *prim, ray_t *ray, bool prev);
void	prim_hit(SCENE, const prim_t *prim, ray_t *ray);

#endif
