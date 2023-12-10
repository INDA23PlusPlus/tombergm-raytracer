#ifndef MAT_H
#define MAT_H

#include "tex.h"
#include "vec.h"

#define MAT_NOATT	1

typedef struct
{
	const tex_t *	tex;
	vec_t		col;
	float		amb;
	float		ref;
	float		tra;
	float		ind;
	int		flg;
} mat_t;

#endif
