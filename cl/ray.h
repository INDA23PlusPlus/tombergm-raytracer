#ifndef RAY_H
#define RAY_H

#include "vec.h"

typedef __constant struct mat_struct	mat_t;
typedef __constant struct prim_struct	prim_t;
typedef struct ray_struct		ray_t;
typedef __constant struct scene_struct	scene_t;

struct ray_struct
{
	prim_t *	prev;	/* Previous object */
	prim_t *	curr;	/* Current object */
	real_t		rc;	/* Recursive coefficient */
	vec3_t		p;	/* Origin */
	vec3_t		d;	/* Direction */
	vec3_t		q;	/* Hit point */
	vec3_t		n;	/* Hit surface normal */
	real_t		l;	/* Hit distance */
	const mat_t *	mat;	/* Hit material */
	vec3_t		tc;	/* Texture / material color */
	real_t		tr;	/* Texture roughness */
	vec2_t		uv;	/* Hit texture coordinates */
	vec3_t		tu;	/* World-space u-vector */
	vec3_t		tv;	/* World-space v-vector */
	vec3_t		c;	/* Value */
};

void	ray_trace(	scene_t *scene,
			vec3_t *c, vec3_t *p, vec3_t *d,
			unsigned *rand);

#endif
