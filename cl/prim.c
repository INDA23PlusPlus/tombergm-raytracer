#include "prim.h"
#include "ray.h"
#include "sph.h"
#include "tri.h"
#include "vec.h"

real_t prim_trace(	const prim_t *prim, vec3_t *p, vec3_t *d,
			real_t m, prim_t *u)
{
	__constant
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
