#ifndef SCENE_H
#define SCENE_H

#include "mat.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"

typedef struct
{
	int		n_tex;
	tex_t *		p_tex;
	int		n_mat;
	mat_t *		p_mat;
	int		n_tri;
	tri_t *		p_tri;
	int		n_sph;
	sph_t *		p_sph;
	int		n_sph_light;
	sph_t *		p_sph_light;
} scene_t;

extern scene_t scene;

void scene_init(void);
void scene_dstr(void);

#endif
