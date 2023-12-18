#ifndef PRIM_H
#define PRIM_H

#include "vec.h"

#define PRIM_TRI	0
#define PRIM_SPH	1

typedef __constant struct prim_struct	prim_t;
typedef struct ray_struct		ray_t;
typedef __constant struct scene_struct	scene_t;

struct prim_struct
{
	int	type;
	__constant
	void *	ptr;
};

real_t	prim_trace(	const prim_t *prim, vec3_t *p, vec3_t *d,
			real_t m, prim_t *u);
void	prim_hit(const prim_t *prim, ray_t *ray);

#endif
