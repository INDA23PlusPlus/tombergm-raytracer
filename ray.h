#ifndef RAY_H
#define RAY_H

#include "mat.h"
#include "vec.h"

typedef struct
{
	void *		prev;		/* Previous object */
	void *		curr;		/* Current object */
	int		depth;		/* Recursion depth */
	vec3_t		p;		/* Origin */
	vec3_t		d;		/* Direction */
	vec3_t		q;		/* Hit point */
	vec3_t		n;		/* Hit surface normal */
	real_t		l;		/* Hit distance */
	const mat_t *	mat;		/* Hit material */
	vec2_t		uv;		/* Hit texture coordinates */
	vec3_t		tu;		/* Hit texture world-space u-vector */
	vec3_t		tv;		/* Hit texture world-space v-vector */
	vec3_t		c;		/* Value */
} ray_t;

void	ray_shade(ray_t *ray);
int	ray_trace(vec3_t *c, vec3_t *p, vec3_t *d, ray_t *src);

#endif
