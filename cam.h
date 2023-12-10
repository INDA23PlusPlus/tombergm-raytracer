#ifndef CAM_H
#define CAM_H

#include "vec.h"

typedef struct
{
	vec_t	p;
	vec_t	uv;
	vec_t	fv;
	float	l;
	float	r;
	float	t;
	float	b;
	float	n;
} cam_t;

#endif
