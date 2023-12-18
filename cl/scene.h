#ifndef SCENE_H
#define SCENE_H

#include "box.h"

typedef __constant struct bih_struct	bih_t;
typedef __constant struct mat_struct	mat_t;
typedef __constant struct prim_struct	prim_t;
typedef __constant struct scene_struct	scene_t;
typedef __constant struct sph_struct	sph_t;
typedef __constant struct tex_struct	tex_t;
typedef __constant struct tri_struct	tri_t;

struct scene_struct
{
	int	n_tex;
	tex_t *	p_tex;
	int	n_mat;
	mat_t *	p_mat;
	int	n_tri;
	tri_t *	p_tri;
	int	n_sph;
	sph_t *	p_sph;
	int	n_prim;
	prim_t *p_prim;
	int	n_bih;
	bih_t *	p_bih;
	box_t	box;
};

#endif
