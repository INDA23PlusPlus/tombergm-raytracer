#include "prim.h"
#include "ray.h"
#include "scene.h"
#include "sph.h"
#include "tri.h"
#include "vec.h"

real_t prim_trace(	SCENE, const prim_t *prim, vec3_t *p, vec3_t *d,
			real_t m, bool prev)
{
	switch (prim->type)
	{
		case PRIM_TRI	: return tri_trace(	TRI(prim->idx), p, d,
							m, prev);
		case PRIM_SPH	: return sph_trace(	SPH(prim->idx), p, d,
							m, prev);
		default		: return INFINITY;
	}
}

void prim_hit(SCENE, const prim_t *prim, ray_t *ray)
{
	switch (prim->type)
	{
		case PRIM_TRI	: return tri_hit(scene, TRI(prim->idx), ray);
		case PRIM_SPH	: return sph_hit(scene, SPH(prim->idx), ray);
		default		: return;
	}
}
