#include <stddef.h>
#include <tgmath.h>
#include "mat.h"
#include "ray.h"
#include "scene.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"
#include "vec.h"

void ray_shade(ray_t *ray)
{
	const mat_t *	mat		= ray->mat;
	real_t		n_cos		= vec3_dot(&ray->d, &ray->n);
	real_t		n_sin_sq;
	vec3_t		tc		= vec3_unit;
	vec3_t		tn;
	real_t		tr		= 1;
	vec3_t		r;

	if (mat_has_tex(mat))
	{
		tex_sample(mat->tex, &ray->uv, &tc, &tn, &tr);

		if (tex_has_n(mat->tex))
		{
			vec3_scale(&ray->n, tn.z, &ray->n);
			vec3_fma(&ray->n, &ray->n, tn.x, &ray->tu);
			vec3_fma(&ray->n, &ray->n, tn.y, &ray->tv);
		}
	}

	{
		vec3_nmul(&tc, &tc, &mat->col);

		tr = tr * mat->dif;
		if (tr > 1) tr = 1;
		if (tr < 0) tr = 0;

		vec3_diffuse(&ray->n, &ray->n, tr * M_PI / 2);
	}

	vec3_sub(&r, &ray->q, &ray->p);

	/* Reflection */
	if (mat->ref != 0)
	{
		vec3_t c;
		vec3_t d;

		vec3_set(&c, &vec3_zero);
		vec3_fma(&d, &ray->d, -2 * n_cos, &ray->n);

		ray_trace(&c, &ray->q, &d, ray);

		vec3_fma(&ray->c, &ray->c, mat->ref, &c);
	}

	/* Transmission */
	if (mat->tra != 0)
	{
		vec3_t c;
		vec3_t d;

		vec3_set(&c, &vec3_zero);

		vec3_scale(&d, n_cos, &ray->n);
		vec3_sub(&d, &ray->d, &d);
		if (n_cos < 0)
		{
			/* Refract in to material */
			vec3_scale(&d, 1 / mat->ind, &d);
		}
		else
		{
			/* Refract out of material */
			vec3_scale(&d, 1 * mat->ind, &d);
		}
		n_sin_sq = vec3_len_sq(&d);

		if (n_sin_sq > 1)
		{
			/* Total internal reflection */
			vec3_fma(&d, &ray->d, -2 * n_cos, &ray->n);
		}
		else
		{
			vec3_fma(&d, &d,
				copysign(sqrt(1 - n_sin_sq), n_cos),
				&ray->n);
		}

		ray_trace(&c, &ray->q, &d, ray);

		vec3_fma(&ray->c, &ray->c, mat->tra, &c);
	}

	/* Ambient */
	{
		vec3_fma(&ray->c, &ray->c, mat->amb, &vec3_unit);
	}

	/* Texture */
	{
		vec3_nmul(&ray->c, &ray->c, &tc);
	}

	/* Attenuation */
	if ((mat->flg & MAT_NOATT) == 0)
	{
		real_t m = 100;
		real_t a = m / vec3_len_sq(&r);

		if (a > 1)
		{
			a = 1;
		}

		vec3_scale(&ray->c, a, &ray->c);
	}
}

int ray_trace(vec3_t *c, vec3_t *p, vec3_t *d, ray_t *src)
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

	vec3_set(&ray.p, p);
	vec3_set(&ray.d, d);
	vec3_set(&ray.c, &vec3_zero);

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
		vec3_add(c, c, &ray.c);
	}

	{
		/* Directional light */
		real_t lm	= 0.50;
		vec3_t lc	= { 1.00, 1.00, 0.95 };
		vec3_t ln	= {  0.1, -1.0,  0.1 };

		vec3_norm(&ln, &ln);

		real_t a = vec3_dot(&ray.d, &ln);

		if (a < 0)
		{
			vec3_fma(c, c, -a * lm, &lc);
		}
	}

	return 0;
}
