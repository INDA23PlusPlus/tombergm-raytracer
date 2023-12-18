#ifndef TEX_H
#define TEX_H

#include "vec.h"

typedef __constant struct tex_struct	tex_t;

struct tex_struct
{
	int	w;
	int	h;
	__constant
	void *	c;
	__constant
	void *	n;
	__constant
	void *	r;
};

static inline int tex_has_n(const tex_t *tex)
{
	return tex->n != NULL;
}

static inline int tex_has_r(const tex_t *tex)
{
	return tex->r != NULL;
}

void	tex_sample(	const tex_t *tex, const vec2_t *uv,
			vec3_t *c, vec3_t *n, real_t *r);

#endif
