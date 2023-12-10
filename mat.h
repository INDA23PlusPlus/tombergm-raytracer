#ifndef MAT_H
#define MAT_H

#include <stddef.h>
#include "tex.h"
#include "vec.h"

#define MAT_FLAT	1

typedef struct
{
	const tex_t *	tex;
	vec3_t		col;
	real_t		dif;
	real_t		amb;
	real_t		ref;
	real_t		tra;
	real_t		ind;
	int		flg;
} mat_t;

static inline int mat_has_tex(const mat_t *mat)
{
	return mat->tex != NULL;
}

#endif
