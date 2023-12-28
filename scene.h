#ifndef SCENE_H
#define SCENE_H

#include "bih.h"
#include "box.h"
#include "mat.h"
#include "prim.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"

typedef struct scene_struct
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
} scene_t;

extern scene_t scene;

void scene_init(void);
void scene_dstr(void);

#endif
