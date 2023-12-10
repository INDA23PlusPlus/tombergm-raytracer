#include <math.h>
#include <stddef.h>
#include "mat.h"
#include "ray.h"
#include "scene.h"
#include "sph.h"
#include "tri.h"
#include "vec.h"

void ray_shade(ray_t *ray)
{
	const mat_t *	mat	= ray->mat;
	float		n_cos	= vec_dot(&ray->d, &ray->n);
	vec_t		r;

	vec_sub(&r, &ray->q, &ray->p);

	/* Reflection */
	if (mat->ref != 0)
	{
		vec_t rc;
		vec_t rd;

		vec_set(&rc, &vec_zero);
		vec_fma(&rd, &ray->d, -2 * n_cos, &ray->n);

		ray_trace(&rc, &ray->q, &rd, ray);

		vec_fma(&ray->c, &ray->c, mat->ref, &rc);
	}

	/* Transmission */
	if (mat->tra != 0)
	{
		vec_t rc;
		vec_t rd;

		vec_set(&rc, &vec_zero);

		vec_scale(&rd, n_cos, &ray->n);
		vec_sub(&rd, &ray->d, &rd);
		vec_scale(&rd, 1 / mat->ind, &rd);
		vec_fma(&rd, &rd,
			copysignf(sqrtf(1 - vec_len_sq(&rd)), n_cos),
			&ray->n);

		ray_trace(&rc, &ray->q, &rd, ray);

		vec_fma(&ray->c, &ray->c, mat->tra, &rc);
	}

	/* Ambient */
	{
		vec_fma(&ray->c, &ray->c, mat->amb, &vec_unit);
	}

	/* Texture */
	{
		vec_t rc = mat->col;

		if (mat->tex != NULL)
		{
			tex_sample(mat->tex, &ray->uv, &rc);
		}

		vec_nmul(&ray->c, &ray->c, &rc);
	}

	/* Attenuation */
	if ((mat->flg & MAT_NOATT) == 0)
	{
		float m = 100;
		float a = m / vec_len_sq(&r);

		if (a > 1)
		{
			a = 1;
		}

		vec_scale(&ray->c, a, &ray->c);
	}
}

int ray_trace(vec_t *c, vec_t *p, vec_t *d, ray_t *src)
{
	ray_t ray;

	ray.prev	= NULL;
	ray.curr	= NULL;
	ray.depth	= 0;
	ray.l		= 1. / 0.;
	ray.mat		= NULL;

	if (src != NULL)
	{
		ray.prev	= src->curr;
		ray.depth	= src->depth + 1;
	}

	if (ray.depth == RAY_DEPTH)
	{
		return 0;
	}

	vec_set(&ray.p, p);
	vec_set(&ray.d, d);
	vec_set(&ray.c, &vec_zero);

	for (int i = 0; i < scene.n_tri; i++)
	{
		tri_t *tri = &scene.tri[i];

		tri_trace(tri, &ray);
	}

	for (int i = 0; i < scene.n_sph; i++)
	{
		sph_t *sph = &scene.sph[i];

		sph_trace(sph, &ray);
	}

	for (int i = 0; i < scene.n_sph_light; i++)
	{
		sph_t *sph = &scene.sph_light[i];

		ray_sph_light(sph, c, p, d, &ray.l, ray.depth + 1);
	}

	if (ray.mat != NULL)
	{
		ray_shade(&ray);
		vec_add(c, c, &ray.c);
	}

	{
		/* Directional light */
		float lm	= 0.4;
		vec_t lc	= { 1, 1, 1 };
		vec_t ln	= { 0.1, -1, 0.1 };

		vec_norm(&ln, &ln);

		float a = vec_dot(&ray.d, &ln);

		if (a < 0)
		{
			vec_fma(c, c, -a * lm, &lc);
		}
	}

	return 0;
}
