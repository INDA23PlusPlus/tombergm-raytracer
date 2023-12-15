#ifndef TRI_H
#define TRI_H

#include "vec.h"

typedef struct mat_struct	mat_t;
typedef struct ray_struct	ray_t;
typedef struct tri_struct	tri_t;

struct tri_struct
{
	vec3_t		a;
	vec3_t		b;
	vec3_t		c;

	const mat_t *	mat;

	vec2_t		at;
	vec2_t		bt;
	vec2_t		ct;

	/* Precomputed values */
	struct
	{
		vec3_t	n;	/* Normal vector */
		vec3_t	i;	/* 1st orthonormal basis vector in plane */
		vec3_t	j;	/* 2nd orthonormal basis vector in plane */
		vec2_t	pa;	/* Vertices in plane basis */
		vec2_t	pb;	/* Vertices in plane basis */
		vec2_t	pc;	/* Vertices in plane basis */
		real_t	d;	/* 1 / det [(pa - pc) (pb - pc)] */
		vec2_t	iv;	/* bt - at */
		vec2_t	jv;	/* ct - at */
		vec3_t	iw;	/* b - a */
		vec3_t	jw;	/* c - a */
		real_t	td;	/* 1 / |(b - a) x (c - a)|^2 */
		vec3_t	tu;	/* Normalized texture world-space u-vector */
		vec3_t	tv;	/* Normalized texture world-space v-vector */
	};
};

static inline void tri_trace(tri_t *tri, ray_t *ray);

#endif
