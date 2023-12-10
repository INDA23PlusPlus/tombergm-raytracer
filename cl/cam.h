#ifndef CAM_H
#define CAM_H

#include "vec.h"

typedef struct cam_struct	cam_t;

struct cam_struct
{
	vec3_t	p;
	vec3_t	uv;
	vec3_t	fv;
	real_t	l;
	real_t	r;
	real_t	t;
	real_t	b;
	real_t	n;
};

#endif
