#ifndef MAT_H
#define MAT_H

#include "tex.h"
#include "vec.h"

#define MAT_FLAT	1

typedef __constant struct tex_struct	tex_t;
typedef __constant struct mat_struct	mat_t;

struct mat_struct
{
	int		tex;
	int		sha;
	char		_pad0[8];
	vec3_t		col;
	real_t		dif;
	real_t		amb;
	real_t		ref;
	real_t		tra;
	real_t		ind;
	int		flg;
	char		_pad1[8];
};

static inline int mat_has_tex(const mat_t *mat)
{
	return mat->tex != TEX_NULL;
}

#endif
