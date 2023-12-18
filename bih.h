#ifndef BIH_H
#define BIH_H

#include "vec.h"

typedef struct bih_struct	bih_t;
typedef struct scene_struct	scene_t;
typedef struct prim_struct	prim_t;

struct bih_struct
{
	int		val;
	union
	{
		real_t	clip[2];
		int	num;
	};
};

void	bih_build(scene_t *scene);
prim_t *bih_trace(scene_t *scene, vec3_t *p, vec3_t *d, real_t *m, prim_t *u);

#endif
