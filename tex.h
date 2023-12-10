#ifndef TEX_H
#define TEX_H

#include "vec.h"

typedef struct
{
	int		w;
	int		h;
	unsigned char *	d;
} tex_t;

int	tex_load(tex_t *tex, const char *filename);
void	tex_dstr(tex_t *tex);
void	tex_sample(const tex_t *tex, const vec2_t *uv, vec_t *c);

#endif
