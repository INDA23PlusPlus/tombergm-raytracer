#ifndef SCENE_H
#define SCENE_H

#include "box.h"

typedef struct bih_struct	bih_t;
typedef struct mat_struct	mat_t;
typedef struct prim_struct	prim_t;
typedef struct scene_struct	scene_t;
typedef struct sph_struct	sph_t;
typedef struct tex_struct	tex_t;
typedef struct tri_struct	tri_t;

struct scene_struct
{
	int		n_tex;
	tex_t *		p_tex;
	int		n_mat;
	mat_t *		p_mat;
	int		n_tri;
	tri_t *		p_tri;
	int		n_sph;
	sph_t *		p_sph;
	int		n_prim;
	prim_t *	p_prim;
	int		n_bih;
	bih_t *		p_bih;
	box_t		box;
};

void scene_init(scene_t *scene);
void scene_dstr(scene_t *scene);

#endif
