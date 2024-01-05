#ifndef BIH_H
#define BIH_H

#include "box.h"
#include "scene.h"
#include "vec.h"

typedef __constant struct bih_struct	bih_t;
typedef __constant struct prim_struct	prim_t;

struct bih_struct
{
	int		val;
	int		num;
	box_t		box;
};

int	bih_trace(SCENE, vec3_t *p, vec3_t *d, real_t *m, int u);

#endif
