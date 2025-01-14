#ifndef RAY_H
#define RAY_H

#include "vec.h"
#include "scene.h"

typedef __constant struct mat_struct	mat_t;
typedef __constant struct prim_struct	prim_t;
typedef struct ray_struct		ray_t;

struct ray_struct
{
	vec3_t		p;	/* Origin */
	vec3_t		d;	/* Direction */
	vec3_t		q;	/* Hit point */
	vec3_t		n;	/* Hit surface normal */
	vec3_t		tc;	/* Texture / material color */
	vec3_t		tu;	/* World-space u-vector */
	vec3_t		tv;	/* World-space v-vector */
	vec3_t		c;	/* Value */
	vec2_t		uv;	/* Hit texture coordinates */
	real_t		l;	/* Hit distance */
	real_t		rc;	/* Recursive coefficient */
	real_t		tr;	/* Texture roughness */
	int		prev;	/* Previous object */
	int		curr;	/* Current object */
	int		mat;	/* Hit material */
};

void	ray_trace(SCENE, vec3_t *c, vec3_t *p, vec3_t *d, unsigned *rand);

#endif
