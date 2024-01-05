#ifndef CLDATA_H
#define CLDATA_H

#include <CL/opencl.h>

typedef struct scene_struct	scene_t;

typedef cl_float		real_cl_t;
typedef cl_float2		vec2_cl_t;
typedef cl_float3		vec3_cl_t;

typedef struct
{
	real_cl_t		min[3];
	real_cl_t		max[3];
} box_cl_t;

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
	char			_pad0[12];
} cam_cl_t;

typedef struct
{
	cl_int			w;
	cl_int			h;
	cl_int			c;
	cl_int			n;
	cl_int			r;
} tex_cl_t;

typedef struct
{
	cl_int			tex;
	char			_pad0[12];
	vec3_cl_t		col;
	real_cl_t		dif;
	real_cl_t		amb;
	real_cl_t		ref;
	real_cl_t		tra;
	real_cl_t		ind;
	cl_int			flg;
	char			_pad1[8];
} mat_cl_t;

typedef struct
{
	vec3_cl_t		a;
	vec3_cl_t		b;
	vec3_cl_t		c;

	cl_int			mat;

	char			_pad0[4];
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
		char		_pad1[4];
		vec2_cl_t	iv;
		vec2_cl_t	jv;
		vec3_cl_t	iw;
		vec3_cl_t	jw;
		real_cl_t	td;
		char		_pad2[12];
		vec3_cl_t	tu;
		vec3_cl_t	tv;
	};
} tri_cl_t;

typedef struct
{
	vec3_cl_t		c;
	real_cl_t		r;

	cl_int			mat;

	char			_pad0[8];
} sph_cl_t;

typedef struct
{
	cl_int			type;
	cl_int			idx;
} prim_cl_t;

typedef struct
{
	cl_int			val;
	cl_int			num;
	box_cl_t		box;
} bih_cl_t;

typedef struct
{
	cl_int			n_tex;
	tex_cl_t *		p_tex;
	cl_mem			m_tex;
	cl_int			n_mat;
	mat_cl_t *		p_mat;
	cl_mem			m_mat;
	cl_int			n_tri;
	tri_cl_t *		p_tri;
	cl_mem			m_tri;
	cl_int			n_sph;
	sph_cl_t *		p_sph;
	cl_mem			m_sph;
	cl_int			n_prim;
	prim_cl_t *		p_prim;
	cl_mem			m_prim;
	cl_int			n_bih;
	bih_cl_t *		p_bih;
	cl_mem			m_bih;
	void *			p_img;
	cl_mem			m_img;
	box_cl_t *		p_box;
	cl_mem			m_box;
} scene_cl_t;

void *cldata_create_scene(	cl_context c, cl_command_queue q,
				const scene_t *scene);

#endif
