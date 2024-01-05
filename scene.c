#include "bih.h"
#include "mat.h"
#include "prim.h"
#include "scene.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"

static tex_t tex[] =
{
	{
		.c = "./res/Daylight Box UV.png",
	},
	{
		.c = "./res/wood_0018_color_4k.jpg",
		.n = "./res/wood_0018_normal_opengl_4k.jpg",
		.r = "./res/wood_0018_roughness_4k.jpg",
	},
};

static mat_t mat[] =
{
	{
		.tex	= &tex[0],
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 0.00,
		.amb	= 1.00,
		.ref	= 0.00,
		.tra	= 0.00,
		.ind	= 1.00,
		.flg	= MAT_FLAT,
	},
	{
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 0.00,
		.amb	= 100.,
		.ref	= 0.00,
		.tra	= 1.00,
		.ind	= 1.00,
	},
	{
		.tex	= &tex[1],
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 1.00,
		.amb	= 0.00,
		.ref	= 0.50,
		.tra	= 0.00,
		.ind	= 1.00,
	},
	{
		.col	= { 1.0, 0.5, 0.5 },
		.dif	= 0.01,
		.amb	= 0.00,
		.ref	= 0.25,
		.tra	= 0.75,
		.ind	= 1.20,
	},
	{
		.col	= { 0.4, 0.4, 0.4 },
		.dif	= 0.20,
		.amb	= 0.00,
		.ref	= 0.80,
		.tra	= 0.00,
		.ind	= 1.00,
	},
	{
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 0.00,
		.amb	= 0.00,
		.ref	= 0.80,
		.tra	= 0.80,
		.ind	= 1.52,
	},
};

static tri_t tri[] =
{
#if 1
#define L 100
	/* Right */
	{
		{  L,  L,  L },
		{  L,  L, -L },
		{  L, -L, -L },
		&mat[0],
		{ 2. / 4., 2. / 3. },
		{ 3. / 4., 2. / 3. },
		{ 3. / 4., 1. / 3. },
	},
	{
		{  L,  L,  L },
		{  L, -L, -L },
		{  L, -L,  L },
		&mat[0],
		{ 2. / 4., 2. / 3. },
		{ 3. / 4., 1. / 3. },
		{ 2. / 4., 1. / 3. },
	},
	/* Left */
	{
		{ -L,  L, -L },
		{ -L,  L,  L },
		{ -L, -L,  L },
		&mat[0],
		{ 0. / 4., 2. / 3. },
		{ 1. / 4., 2. / 3. },
		{ 1. / 4., 1. / 3. },
	},
	{
		{ -L,  L, -L },
		{ -L, -L,  L },
		{ -L, -L, -L },
		&mat[0],
		{ 0. / 4., 2. / 3. },
		{ 1. / 4., 1. / 3. },
		{ 0. / 4., 1. / 3. },
	},
	/* Top */
	{
		{ -L,  L, -L },
		{  L,  L, -L },
		{  L,  L,  L },
		&mat[0],
		{ 1. / 4., 3. / 3. },
		{ 2. / 4., 3. / 3. },
		{ 2. / 4., 2. / 3. },
	},
	{
		{ -L,  L, -L },
		{  L,  L,  L },
		{ -L,  L,  L },
		&mat[0],
		{ 1. / 4., 3. / 3. },
		{ 2. / 4., 2. / 3. },
		{ 1. / 4., 2. / 3. },
	},
	/* Bottom */
	{
		{ -L, -L,  L },
		{  L, -L,  L },
		{  L, -L, -L },
		&mat[0],
		{ 1. / 4., 1. / 3. },
		{ 2. / 4., 1. / 3. },
		{ 2. / 4., 0. / 3. },
	},
	{
		{ -L, -L,  L },
		{  L, -L, -L },
		{ -L, -L, -L },
		&mat[0],
		{ 1. / 4., 1. / 3. },
		{ 2. / 4., 0. / 3. },
		{ 1. / 4., 0. / 3. },
	},
	/* Front */
	{
		{ -L,  L,  L },
		{  L,  L,  L },
		{  L, -L,  L },
		&mat[0],
		{ 1. / 4., 2. / 3. },
		{ 2. / 4., 2. / 3. },
		{ 2. / 4., 1. / 3. },
	},
	{
		{ -L,  L,  L },
		{  L, -L,  L },
		{ -L, -L,  L },
		&mat[0],
		{ 1. / 4., 2. / 3. },
		{ 2. / 4., 1. / 3. },
		{ 1. / 4., 1. / 3. },
	},
	/* Back */
	{
		{  L,  L, -L },
		{ -L,  L, -L },
		{ -L, -L, -L },
		&mat[0],
		{ 3. / 4., 2. / 3. },
		{ 4. / 4., 2. / 3. },
		{ 4. / 4., 1. / 3. },
	},
	{
		{  L,  L, -L },
		{ -L, -L, -L },
		{  L, -L, -L },
		&mat[0],
		{ 3. / 4., 2. / 3. },
		{ 4. / 4., 1. / 3. },
		{ 3. / 4., 1. / 3. },
	},
#undef L
#endif

#if 1
	{
		{ -10,  -1,  10 },
		{  10,  -1,  10 },
		{  10,  -1, -10 },
		&mat[2],
		{ 0.0, 4.0 },
		{ 4.0, 4.0 },
		{ 4.0, 0.0 },
	},
	{
		{ -10,  -1,  10 },
		{  10,  -1, -10 },
		{ -10,  -1, -10 },
		&mat[2],
		{ 0.0, 4.0 },
		{ 4.0, 0.0 },
		{ 0.0, 0.0 },
	},
#endif

#if 1
	{
		{ -1, -1,  2 },
		{  0,  1,  2 },
		{  1, -1,  2 },
		&mat[3],
		{ 0.0, 0.0 },
		{ 0.5, 1.0 },
		{ 1.0, 0.0 },
	},
#endif

//#include "mdl.c"
};

static sph_t sph[] =
{
#if 1
	{
		{ -2.0,  7.0, -2.0 },
		1.0,
		&mat[1],
	},
#endif

#if 1
	{
		{  0.00, -0.50,  3.00 },
		0.50,
		&mat[4],
	},
	{
		{  2.00, -0.25,  1.00 },
		0.75,
		&mat[5],
	}
#endif
};

scene_t scene =
{
	.n_tex		= sizeof(tex) / sizeof*(tex),
	.p_tex		= tex,
	.n_mat		= sizeof(mat) / sizeof*(mat),
	.p_mat		= mat,
	.n_tri		= sizeof(tri) / sizeof*(tri),
	.p_tri		= tri,
	.n_sph		= sizeof(sph) / sizeof*(sph),
	.p_sph		= sph,
};

void scene_init(scene_t *scene)
{
	for (int i = 0; i < scene->n_tex; i++)
	{
		tex_t *tex = &scene->p_tex[i];

		if (tex->c != NULL)
		{
			tex_load_c(tex, tex->c);
		}

		if (tex->n != NULL)
		{
			tex_load_n(tex, tex->n);
		}

		if (tex->r != NULL)
		{
			tex_load_r(tex, tex->r);
		}
	}

	for (int i = 0; i < scene->n_tri; i++)
	{
		tri_t *tri = &scene->p_tri[i];

		tri_precomp(tri);
	}

	prim_build(scene);
	bih_build(scene);
}

void scene_dstr(scene_t *scene)
{
	for (int i = 0; i < scene->n_tex; i++)
	{
		tex_dstr(&scene->p_tex[i]);
	}
}
