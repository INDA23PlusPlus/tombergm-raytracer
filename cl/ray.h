#ifndef RAY_H
#define RAY_H

#include "vec.h"

typedef struct mat_struct	mat_t;
typedef struct scene_struct	scene_t;
typedef struct ray_struct	ray_t;

struct ray_struct
{
	void *		prev;	/* Previous object */
	void *		curr;	/* Current object */

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
	real_t		rc;	/* Recursive coefficient */
};

static inline
void ray_trace(	__constant scene_t *scene,
		vec3_t *c, vec3_t *p, vec3_t *d,
		unsigned *rand);

#endif
