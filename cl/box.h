#ifndef BOX_H
#define BOX_H

#include "vec.h"

typedef struct box_struct		box_t;
typedef struct ray_struct		ray_t;

struct box_struct
{
	real_t	min[3];
	real_t	max[3];
};

real_t	box_trace(__constant const box_t *box, ray_t *ray);

#endif
