#ifndef SCENE_H
#define SCENE_H

typedef struct tex_struct	tex_t;
typedef struct mat_struct	mat_t;
typedef struct tri_struct	tri_t;
typedef struct sph_struct	sph_t;
typedef struct scene_struct	scene_t;

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
	int	n_sph_light;
	sph_t *	p_sph_light;
};

#endif
