#ifndef RAY_H
#define RAY_H

#include "mat.h"
#include "vec.h"

#define RAY_DEPTH 4

typedef struct
{
	void *		prev;		/* Previous object */
	void *		curr;		/* Current object */
	int		depth;		/* Recursion depth */
	vec_t		p;		/* Origin */
	vec_t		d;		/* Direction */
	vec_t		q;		/* Hit point */
	vec_t		n;		/* Hit surface normal */
	float		l;		/* Hit distance */
	const mat_t *	mat;		/* Hit material */
	vec2_t		uv;		/* Hit texture coordinates */
	vec_t		c;		/* Value */
} ray_t;

void	ray_shade(ray_t *ray);
int	ray_trace(vec_t *c, vec_t *p, vec_t *d, ray_t *src);

#endif
