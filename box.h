#ifndef BOX_H
#define BOX_H

#include "vec.h"

typedef struct
{
	real_t	min[3];
	real_t	max[3];
} box_t;

void	box_join(box_t *a, const box_t *b);
real_t	box_trace(const box_t *box, vec3_t *p, vec3_t *d, real_t m);

#endif
