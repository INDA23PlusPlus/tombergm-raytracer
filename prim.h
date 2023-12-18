#ifndef PRIM_H
#define PRIM_H

#include "vec.h"

#define PRIM_TRI	0
#define PRIM_SPH	1

typedef struct box_struct	box_t;
typedef struct prim_struct	prim_t;
typedef struct ray_struct	ray_t;
typedef struct scene_struct	scene_t;

struct prim_struct
{
	int	type;
	void *	ptr;
};

void	prim_build(scene_t *scene);
void	prim_get_box(const prim_t *prim, box_t *box);
real_t	prim_trace(	const prim_t *prim, vec3_t *p, vec3_t *d,
			real_t m, prim_t *u);
void	prim_hit(const prim_t *prim, ray_t *ray);

#endif
