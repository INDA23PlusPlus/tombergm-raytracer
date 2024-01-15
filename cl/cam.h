#ifndef CAM_H
#define CAM_H

#include "vec.h"

typedef __constant struct cam_struct	cam_t;

struct cam_struct
{
	vec3_t	p;	/* Position */
	vec3_t	uv;	/* Up vector */
	vec3_t	fv;	/* Forward vector */
	real_t	l;	/* Left plane */
	real_t	r;	/* Right plane */
	real_t	t;	/* Top plane */
	real_t	b;	/* Bottom plane */
	real_t	n;	/* Near plane */
	real_t	ap;	/* Aperture width */
	char	_pad0[8];
};

#endif
