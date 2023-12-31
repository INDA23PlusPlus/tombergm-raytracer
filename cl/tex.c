#include "tex.h"
#include "vec.h"

#define BILERP

static void load_sample(__constant void *buf, int w, int h,
			int x, int y, vec3_t *s)
{
	__constant
	unsigned char *p = buf;

	if (x < 0) x = w + x % w;
	if (y < 0) y = h + y % h;

	x = x % w;
	y = y % h;

	p = &p[((h - 1 - y) * w + x) * 3];

	s->x = p[0] / 255.f;
	s->y = p[1] / 255.f;
	s->z = p[2] / 255.f;
}

__attribute__((unused))
static void nearest(	__constant void *buf, int w, int h,
			const vec2_t *uv, vec3_t *s)
{
	load_sample(buf, w, h, uv->x * (w - 1), uv->y * (h - 1), s);
}

__attribute__((unused))
static void bilerp(	__constant void *buf, int w, int h,
			const vec2_t *uv, vec3_t *s)
{
	real_t	fx	= uv->x * (w - 1);
	real_t	fy	= uv->y * (h - 1);
	int	x	= fx;
	int	y	= fy;
	vec3_t	s0[4];
	vec3_t	s1[2];

	fx = fx - x;
	fy = fy - y;

	load_sample(buf, w, h, x + 0, y + 0, &s0[0]);
	load_sample(buf, w, h, x + 1, y + 0, &s0[1]);
	load_sample(buf, w, h, x + 0, y + 1, &s0[2]);
	load_sample(buf, w, h, x + 1, y + 1, &s0[3]);

	s1[0] = s0[1] - s0[0];
	s1[0] = s0[0] + fx * s1[0];

	s1[1] = s0[3] - s0[2];
	s1[1] = s0[2] + fx * s1[1];

	*s = s1[1] - s1[0];
	*s = s1[0] + fy * *s;
}

void tex_sample(const tex_t *tex, const vec2_t *uv,
		vec3_t *c, vec3_t *n, real_t *r)
{
	if (c != NULL && tex->c != NULL)
	{
#ifdef BILERP
		bilerp(tex->c, tex->w, tex->h, uv, c);
#else
		nearest(tex->c, tex->w, tex->h, uv, c);
#endif
	}

	if (n != NULL && tex->n != NULL)
	{
#ifdef BILERP
		bilerp(tex->n, tex->w, tex->h, uv, n);

		*n = *n * 2 - 1;

		*n = normalize(*n);
#else
		nearest(tex->n, tex->w, tex->h, uv, n);

		*n = *n * 2 - 1;
#endif
	}

	if (r != NULL && tex->r != NULL)
	{
		vec3_t tr;

#ifdef BILERP
		bilerp(tex->r, tex->w, tex->h, uv, &tr);
#else
		nearest(tex->r, tex->w, tex->h, uv, &tr);
#endif

		*r = tr.x;
	}
}
