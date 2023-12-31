#include <stdio.h>
#include <tgmath.h>
#include "box.h"
#include "prim.h"
#include "ray.h"
#include "scene.h"
#include "sph.h"
#include "tri.h"
#include "vec.h"
#include "vector.h"

void prim_build(scene_t *scene)
{
	vector_t v;

	vector_init(&v, sizeof(prim_t));

	for (int i = 0; i < scene->n_tri; i++)
	{
		prim_t prim;

		prim.type = PRIM_TRI;
		prim.ptr = &scene->p_tri[i];

		vector_push(&v, &prim);
	}

	for (int i = 0; i < scene->n_sph; i++)
	{
		prim_t prim;

		prim.type = PRIM_SPH;
		prim.ptr = &scene->p_sph[i];

		vector_push(&v, &prim);
	}

	scene->n_prim = v.num;
	scene->p_prim = v.buf;

	fprintf(stderr, "Primitives: %i\n", scene->n_prim);
}

void prim_get_box(const prim_t *prim, box_t *box)
{
	switch (prim->type)
	{
		case PRIM_TRI	: return tri_get_box(prim->ptr, box);
		case PRIM_SPH	: return sph_get_box(prim->ptr, box);
		default		: return;
	}
}

real_t prim_trace(	const prim_t *prim, vec3_t *p, vec3_t *d,
			real_t m, prim_t *u)
{
	void *	prev;

	if (u != NULL)
	{
		prev = u->ptr;
	}
	else
	{
		prev = NULL;
	}

	switch (prim->type)
	{
		case PRIM_TRI	: return tri_trace(prim->ptr, p, d, m, prev);
		case PRIM_SPH	: return sph_trace(prim->ptr, p, d, m, prev);
		default		: return INFINITY;
	}
}

void prim_hit(const prim_t *prim, ray_t *ray)
{
	switch (prim->type)
	{
		case PRIM_TRI	: return tri_hit(prim->ptr, ray);
		case PRIM_SPH	: return sph_hit(prim->ptr, ray);
		default		: return;
	}
}
