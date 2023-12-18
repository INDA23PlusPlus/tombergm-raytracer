#ifndef RAY_H
#define RAY_H

#include "vec.h"

typedef struct mat_struct	mat_t;
typedef struct prim_struct	prim_t;
typedef struct ray_struct	ray_t;
typedef struct scene_struct	scene_t;

struct ray_struct
{
	prim_t *	prev;		/* Previous object */
	prim_t *	curr;		/* Current object */
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
};

int	ray_trace(scene_t *scene, vec3_t *c, vec3_t *p, vec3_t *d, ray_t *src);

#endif
