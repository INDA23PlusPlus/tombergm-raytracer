#ifndef CLDATA_H
#define CLDATA_H

#include <CL/opencl.h>
#include "scene.h"

typedef cl_float	real_cl_t;
typedef cl_float2	vec2_cl_t;
typedef cl_float3	vec3_cl_t;

typedef struct
{
	vec3_cl_t		p;
	vec3_cl_t		uv;
	vec3_cl_t		fv;
	real_cl_t		l;
	real_cl_t		r;
	real_cl_t		t;
	real_cl_t		b;
	real_cl_t		n;
} cam_cl_t;

typedef struct
{
	cl_int			w;
	cl_int			h;
	unsigned char *		c;
	unsigned char *		n;
	unsigned char *		r;
} tex_cl_t;

typedef struct
{
	const tex_cl_t *	tex;
	vec3_cl_t		col;
	real_cl_t		dif;
	real_cl_t		amb;
	real_cl_t		ref;
	real_cl_t		tra;
	real_cl_t		ind;
	cl_int			flg;
} mat_cl_t;

typedef struct
{
	vec3_cl_t		a;
	vec3_cl_t		b;
	vec3_cl_t		c;

	const mat_cl_t *	mat;

	vec2_cl_t		at;
	vec2_cl_t		bt;
	vec2_cl_t		ct;

	struct
	{
		vec3_cl_t	n;
		vec3_cl_t	i;
		vec3_cl_t	j;
		vec2_cl_t	pa;
		vec2_cl_t	pb;
		vec2_cl_t	pc;
		real_cl_t	d;
		vec2_cl_t	iv;
		vec2_cl_t	jv;
		vec3_cl_t	iw;
		vec3_cl_t	jw;
		real_cl_t	td;
		vec3_cl_t	tu;
		vec3_cl_t	tv;
	};
} tri_cl_t;

typedef struct
{
	vec3_cl_t		c;
	real_cl_t		r;

	const mat_cl_t *	mat;
} sph_cl_t;

typedef struct
{
	cl_int			n_tex;
	tex_cl_t *		p_tex;
	cl_int			n_mat;
	mat_cl_t *		p_mat;
	cl_int			n_tri;
	tri_cl_t *		p_tri;
	cl_int			n_sph;
	sph_cl_t *		p_sph;
	cl_int			n_sph_light;
	sph_cl_t *		p_sph_light;
} scene_cl_t;

void *cldata_create_scene(	cl_context c, cl_command_queue q,
				const scene_t *scene);

void cldata_set_kernel_bufs(cl_kernel k);

#endif
