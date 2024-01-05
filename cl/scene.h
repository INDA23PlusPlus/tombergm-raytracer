#ifndef SCENE_H
#define SCENE_H

#include "box.h"

typedef __constant struct bih_struct	bih_t;
typedef __constant struct mat_struct	mat_t;
typedef __constant struct prim_struct	prim_t;
typedef __constant struct sph_struct	sph_t;
typedef __constant struct tex_struct	tex_t;
typedef __constant struct tri_struct	tri_t;

#define SCENE \
	int	scene_n_tex, \
	tex_t *	scene_p_tex, \
	int	scene_n_mat, \
	mat_t *	scene_p_mat, \
	int	scene_n_tri, \
	tri_t *	scene_p_tri, \
	int	scene_n_sph, \
	sph_t *	scene_p_sph, \
	int	scene_n_prim, \
	prim_t *scene_p_prim, \
	int	scene_n_bih, \
	bih_t *	scene_p_bih, \
	image2d_array_t	scene_i_img, \
	__constant \
	box_t *	scene_p_box

#define scene \
	scene_n_tex, \
	scene_p_tex, \
	scene_n_mat, \
	scene_p_mat, \
	scene_n_tri, \
	scene_p_tri, \
	scene_n_sph, \
	scene_p_sph, \
	scene_n_prim, \
	scene_p_prim, \
	scene_n_bih, \
	scene_p_bih, \
	scene_i_img, \
	scene_p_box

#define TEX(n)	(&scene_p_tex[n])
#define MAT(n)	(&scene_p_mat[n])
#define TRI(n)	(&scene_p_tri[n])
#define SPH(n)	(&scene_p_sph[n])
#define PRIM(n)	(&scene_p_prim[n])
#define BIH(n)	(&scene_p_bih[n])
#define IMG	scene_i_img

#endif
