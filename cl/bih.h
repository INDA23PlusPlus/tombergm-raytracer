#ifndef BIH_H
#define BIH_H

#include "box.h"
#include "scene.h"
#include "vec.h"

typedef __constant struct bih_struct	bih_t;
typedef __constant struct prim_struct	prim_t;
typedef struct ray_struct		ray_t;

struct bih_struct
{
	box_t		box;
	int		next;
	int		prim_idx;
	int		prim_num;
};

void	bih_trace(SCENE, ray_t *ray);

#endif
