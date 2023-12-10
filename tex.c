#include <stddef.h>
#include <stdlib.h>
#include "stb_image.h"
#include "tex.h"
#include "vec.h"

int tex_load(tex_t *tex, const char *filename)
{
	int n;

	tex->d = stbi_load(filename, &tex->w, &tex->h, &n, 3);

	return tex->d != NULL;
}

void tex_dstr(tex_t *tex)
{
	if (tex->d != NULL)
	{
		free(tex->d);

		tex->d = NULL;
	}
}

static void load_sample(const tex_t *tex, int x, int y, vec_t *c)
{
	int i;

	if (x < 0)		x = 0;
	if (x > tex->w - 1)	x = tex->w - 1;
	if (y < 0)		y = 0;
	if (y > tex->h - 1)	y = tex->h - 1;

	i = ((tex->h - 1 - y) * tex->w + x) * 3;

	c->x = tex->d[i + 0] / 255.f;
	c->y = tex->d[i + 1] / 255.f;
	c->z = tex->d[i + 2] / 255.f;
}

void tex_sample(const tex_t *tex, const vec2_t *uv, vec_t *c)
{
	float	fx	= uv->x * (tex->w - 1);
	float	fy	= uv->y * (tex->h - 1);
	int	x	= fx;
	int	y	= fy;

	if (x >= 0 && x < tex->w && y >= 0 && y < tex->h)
	{
		vec_t	s0[4];
		vec_t	s1[2];

		load_sample(tex, x + 0, y + 0, &s0[0]);
		load_sample(tex, x + 1, y + 0, &s0[1]);
		load_sample(tex, x + 0, y + 1, &s0[2]);
		load_sample(tex, x + 1, y + 1, &s0[3]);

		fx = fx - x;
		fy = fy - y;

		vec_sub(&s1[0], &s0[1], &s0[0]);
		vec_fma(&s1[0], &s0[0], fx, &s1[0]);

		vec_sub(&s1[1], &s0[3], &s0[2]);
		vec_fma(&s1[1], &s0[2], fx, &s1[1]);

		vec_sub(c, &s1[1], &s1[0]);
		vec_fma(c, &s1[0], fy, c);
	}
}
