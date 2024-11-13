#include "bih.h"
#include "mat.h"
#include "prim.h"
#include "scene.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"

static tex_t tex[] =
{
	/* 0: Skybox */
	{
		.c = "./res/Daylight Box UV.png",
	},
	/* 1: Herringbone wood */
	{
		.c = "./res/wood_0018_color_4k.jpg",
		.n = "./res/wood_0018_normal_opengl_4k.jpg",
		.r = "./res/wood_0018_roughness_4k.jpg",
	},
};

static mat_t mat[] =
{
	/* 0: Sky */
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
	/* 1: Sun */
	{
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 0.00,
		.amb	= 80.,
		.ref	= 0.00,
		.tra	= 1.00,
		.ind	= 1.00,
	},
	/* 2: Herringbone wood */
	{
		.tex	= &tex[1],
		.sha	= 1,
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 1.00,
		.amb	= 0.00,
		.ref	= 0.50,
		.tra	= 0.00,
		.ind	= 1.00,
	},
	/* 3: Red-tinted glass */
	{
		.col	= { 1.0, 0.5, 0.5 },
		.dif	= 0.01,
		.amb	= 0.00,
		.ref	= 0.25,
		.tra	= 0.75,
		.ind	= 1.20,
	},
	/* 4: Shiny gray metal */
	{
		.sha	= 1,
		.col	= { 0.4, 0.4, 0.4 },
		.dif	= 0.05,
		.amb	= 0.00,
		.ref	= 0.00,
		.tra	= 0.00,
		.ind	= 0.95,
	},
	/* 5: Clear glass */
	{
		.col	= { 1.0, 1.0, 1.0 },
		.dif	= 0.00,
		.amb	= 0.00,
		.ref	= 0.80,
		.tra	= 0.80,
		.ind	= 1.52,
	},
	/* 6: Shiny gold metal */
	{
		.sha	= 1,
		.col	= { 0.86, 0.67, 0.20 },
		.dif	= 0.20,
		.amb	= 0.00,
		.ref	= 0.00,
		.tra	= 0.00,
		.ind	= 0.47,
	},
};

static tri_t tri[] =
{
#if 1
#define L 100
	/* Skybox right */
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
	/* Skybox left */
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
	/* Skybox top */
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
	/* Skynox bottom */
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
	/* Skybox front */
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
	/* Skybox back */
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
	/* Wood floor */
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
	/* Red glass triangle */
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

#include "mdl/glass-cup.c"
};

static sph_t sph[] =
{
#if 1
	/* Sun */
	{
		{ -2.0,  7.0, -2.0 },
		1.0,
		&mat[1],
	},
#endif

#if 1
	/* Big transparent marble */
	{
		{  0.00, -0.50,  3.00 },
		0.50,
		&mat[4],
	},
	/* Shiny metal ball */
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
