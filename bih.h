#ifndef BIH_H
#define BIH_H

#include "prim.h"
#include "vec.h"

typedef struct scene_struct scene_t;

typedef struct
{
	int		val;
	union
	{
		real_t	clip[2];
		int	num;
	};
} bih_t;

void	bih_build(scene_t *scene);
void *	bih_trace(scene_t *scene, vec3_t *p, vec3_t *d, real_t *m, prim_t *u);

#endif
