#ifndef TEX_H
#define TEX_H

#include "scene.h"
#include "vec.h"

typedef __constant struct tex_struct	tex_t;

struct tex_struct
{
	int	w;
	int	h;
	int	c;
	int	n;
	int	r;
};

static inline int tex_has_n(const tex_t *tex)
{
	return tex->n != -1;
}

static inline int tex_has_r(const tex_t *tex)
{
	return tex->r != -1;
}

void	tex_sample(	SCENE, const tex_t *tex, const vec2_t *uv,
			vec3_t *c, vec3_t *n, real_t *r);

#endif
