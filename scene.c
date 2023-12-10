#include <stddef.h>
#include "mat.h"
#include "scene.h"
#include "sph.h"
#include "tri.h"

static tex_t tex[] =
{
	{},
	{},
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
		.dif	= 0.40,
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
		&mat[1],
		{ 0.0, 4.0 },
		{ 4.0, 4.0 },
		{ 4.0, 0.0 },
	},
	{
		{ -10,  -1,  10 },
		{  10,  -1, -10 },
		{ -10,  -1, -10 },
		&mat[1],
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
		&mat[2],
		{ 0.0, 0.0 },
		{ 0.5, 1.0 },
		{ 1.0, 0.0 },
	},
#endif
};

static sph_t sph[] =
{
#if 1
	{
		{  0.00, -0.50,  3.00 },
		0.50,
		&mat[3],
	},
	{
		{  2.00, -0.25,  1.00 },
		0.75,
		&mat[4],
	}
#endif
};

static sph_t sph_light[] =
{
#if 1
	{
		{ -2.0,  7.0, -2.0 },
		1.0,
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
	.n_sph_light	= sizeof(sph_light) / sizeof*(sph_light),
	.p_sph_light	= sph_light,
};

void scene_init(void)
{
	tex_load_c(&tex[0], "./res/Daylight Box UV.png");
	tex_load_c(&tex[1], "./res/wood_0018_color_4k.jpg");
	tex_load_n(&tex[1], "./res/wood_0018_normal_opengl_4k.jpg");
	tex_load_r(&tex[1], "./res/wood_0018_roughness_4k.jpg");

	for (int i = 0; i < scene.n_tri; i++)
	{
		tri_t *tri = &scene.p_tri[i];

		tri_precomp(tri);
	}
}


void scene_dstr(void)
{
	tex_dstr(&tex[0]);
}
