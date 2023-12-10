#ifndef SCENE_H
#define SCENE_H

#include "sph.h"
#include "tri.h"

typedef struct
{
	int	n_tri;
	tri_t *	tri;
	int	n_sph;
	sph_t *	sph;
	int	n_sph_light;
	sph_t *	sph_light;
} scene_t;

extern scene_t scene;

void scene_init(void);
void scene_dstr(void);

#endif
