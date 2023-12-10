#include <stddef.h>
#include "mat.h"
#include "scene.h"
#include "sph.h"
#include "tri.h"

static tex_t tex0;
static tex_t tex1;

static mat_t mat0 =
{
	.tex	= &tex0,
	.col	= { 1.0, 1.0, 1.0 },
	.dif	= 0.00,
	.amb	= 1.00,
	.ref	= 0.00,
	.tra	= 0.00,
	.ind	= 1.00,
	.flg	= MAT_NOATT,
};

static mat_t mat1 =
{
	.tex	= &tex1,
	.col	= { 1.0, 1.0, 1.0 },
	.dif	= 1.20,
	.amb	= 0.00,
	.ref	= 0.70,
	.tra	= 0.00,
	.ind	= 1.00,
};

static mat_t mat2 =
{
	.col	= { 1.0, 0.5, 0.5 },
	.dif	= 0.01,
	.amb	= 0.00,
	.ref	= 0.50,
	.tra	= 0.50,
	.ind	= 1.20,
};

static mat_t mat3 =
{
	.col	= { 0.4, 0.3, 0.2 },
	.dif	= 0.01,
	.amb	= 0.00,
	.ref	= 0.60,
	.tra	= 0.00,
	.ind	= 1.00,
};

static mat_t mat4 =
{
	.col	= { 1.0, 1.0, 1.0 },
	.dif	= 0.00,
	.amb	= 0.00,
	.ref	= 0.05,
	.tra	= 0.90,
	.ind	= 1.52,
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
		&mat0,
		{ 2. / 4., 2. / 3. },
		{ 3. / 4., 2. / 3. },
		{ 3. / 4., 1. / 3. },
	},
	{
		{  L,  L,  L },
		{  L, -L, -L },
		{  L, -L,  L },
		&mat0,
		{ 2. / 4., 2. / 3. },
		{ 3. / 4., 1. / 3. },
		{ 2. / 4., 1. / 3. },
	},
	/* Left */
	{
		{ -L,  L, -L },
		{ -L,  L,  L },
		{ -L, -L,  L },
		&mat0,
		{ 0. / 4., 2. / 3. },
		{ 1. / 4., 2. / 3. },
		{ 1. / 4., 1. / 3. },
	},
	{
		{ -L,  L, -L },
		{ -L, -L,  L },
		{ -L, -L, -L },
		&mat0,
		{ 0. / 4., 2. / 3. },
		{ 1. / 4., 1. / 3. },
		{ 0. / 4., 1. / 3. },
	},
	/* Top */
	{
		{ -L,  L, -L },
		{  L,  L, -L },
		{  L,  L,  L },
		&mat0,
		{ 1. / 4., 3. / 3. },
		{ 2. / 4., 3. / 3. },
		{ 2. / 4., 2. / 3. },
	},
	{
		{ -L,  L, -L },
		{  L,  L,  L },
		{ -L,  L,  L },
		&mat0,
		{ 1. / 4., 3. / 3. },
		{ 2. / 4., 2. / 3. },
		{ 1. / 4., 2. / 3. },
	},
	/* Bottom */
	{
		{ -L, -L,  L },
		{  L, -L,  L },
		{  L, -L, -L },
		&mat0,
		{ 1. / 4., 1. / 3. },
		{ 2. / 4., 1. / 3. },
		{ 2. / 4., 0. / 3. },
	},
	{
		{ -L, -L,  L },
		{  L, -L, -L },
		{ -L, -L, -L },
		&mat0,
		{ 1. / 4., 1. / 3. },
		{ 2. / 4., 0. / 3. },
		{ 1. / 4., 0. / 3. },
	},
	/* Front */
	{
		{ -L,  L,  L },
		{  L,  L,  L },
		{  L, -L,  L },
		&mat0,
		{ 1. / 4., 2. / 3. },
		{ 2. / 4., 2. / 3. },
		{ 2. / 4., 1. / 3. },
	},
	{
		{ -L,  L,  L },
		{  L, -L,  L },
		{ -L, -L,  L },
		&mat0,
		{ 1. / 4., 2. / 3. },
		{ 2. / 4., 1. / 3. },
		{ 1. / 4., 1. / 3. },
	},
	/* Back */
	{
		{  L,  L, -L },
		{ -L,  L, -L },
		{ -L, -L, -L },
		&mat0,
		{ 3. / 4., 2. / 3. },
		{ 4. / 4., 2. / 3. },
		{ 4. / 4., 1. / 3. },
	},
	{
		{  L,  L, -L },
		{ -L, -L, -L },
		{  L, -L, -L },
		&mat0,
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
		&mat1,
		{ 0.0, 4.0 },
		{ 4.0, 4.0 },
		{ 4.0, 0.0 },
	},
	{
		{ -10,  -1,  10 },
		{  10,  -1, -10 },
		{ -10,  -1, -10 },
		&mat1,
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
		&mat2,
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
		&mat3,
	},
	{
		{  2.00, -0.25,  1.00 },
		0.75,
		&mat4,
	}
#endif
};

static sph_t sph_light[] =
{
#if 1
	{
		{ -2.0,  7.0, -2.0 },
		2.0,
	}
#endif
};

scene_t scene =
{
	.n_tri		= sizeof(tri) / sizeof*(tri),
	.tri		= tri,
	.n_sph		= sizeof(sph) / sizeof*(sph),
	.sph		= sph,
	.n_sph_light	= sizeof(sph_light) / sizeof*(sph_light),
	.sph_light	= sph_light,
};

void scene_init(void)
{
	tex_load_c(&tex0, "./res/Daylight Box UV.png");
	tex_load_c(&tex1, "./res/Wood_Herringbone_BaseColor.jpg");
	tex_load_n(&tex1, "./res/Wood_Herringbone_Normal.jpg");
	tex_load_r(&tex1, "./res/Wood_Herringbone_Roughness.jpg");

	for (int i = 0; i < scene.n_tri; i++)
	{
		tri_t *tri = &scene.tri[i];

		tri_precomp(tri);
	}
}


void scene_dstr(void)
{
	tex_dstr(&tex0);
}
