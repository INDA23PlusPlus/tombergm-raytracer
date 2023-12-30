#ifndef BIH_H
#define BIH_H

#include "vec.h"

typedef struct bih_struct	bih_t;
typedef struct prim_struct	prim_t;
typedef struct scene_struct	scene_t;

struct bih_struct
{
	int		val;
	union
	{
		real_t	clip[2];
		int	num;
	};
};

void *	bih_trace(	__constant scene_t *scene, vec3_t *p, vec3_t *d,
			real_t *m, prim_t *u);

#endif
