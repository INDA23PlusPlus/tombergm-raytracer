#ifndef MAT_H
#define MAT_H

#include "vec.h"

#define MAT_FLAT	1

typedef __constant struct tex_struct	tex_t;
typedef __constant struct mat_struct	mat_t;

struct mat_struct
{
	const tex_t *	tex;
	vec3_t		col;
	real_t		dif;
	real_t		amb;
	real_t		ref;
	real_t		tra;
	real_t		ind;
	int		flg;
};

static inline int mat_has_tex(const mat_t *mat)
{
	return mat->tex != NULL;
}

#endif
