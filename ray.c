#include <stddef.h>
#include <tgmath.h>
#include "mat.h"
#include "ray.h"
#include "rt.h"
#include "scene.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"
#include "vec.h"

#define RAY_DEPTH 4

void ray_shade(ray_t *ray)
{
	const mat_t *	mat		= ray->mat;
	real_t		n_cos;
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
			vec3_norm(&ray->n, &ray->n);
		}
	}

	n_cos = vec3_dot(&ray->d, &ray->n);
	vec3_nmul(&tc, &tc, &mat->col);
	vec3_sub(&r, &ray->q, &ray->p);

	float ref_rc = ray->mat->ref;
	float tra_rc = tr;
	float ind;
	float R;

	if (n_cos < 0)
	{
		ind = mat->ind;
	}
	else
	{
		ind = 1. / mat->ind;
	}

	{
		/* Fresnel equation for non-magnetic reflectivity */
		float b = ind * ind + n_cos * n_cos - 1;

		if (b < 0)
		{
			/* Total internal reflection */
			R = 1.0 / 0.0;
			ref_rc = 1;
		}
		else
		{
			b = sqrt(b);
			R = (fabs(n_cos) - b) / (fabs(n_cos) + b);
			R = R * R;
		}
	}

	if (flt_rand() * mat->tra <= R)
	{
		if (flt_rand() * tr < mat->dif)
		{
			/* Diffuse reflection */
			vec3_t c;
			vec3_t d;

			vec3_set(&c, &vec3_zero);
			vec3_diffuse(&d, &ray->n, M_PI / 2);

			ray_trace(&c, &ray->q, &d, ray);

			vec3_fma(&ray->c, &ray->c,
					ref_rc
					* fabs(vec3_dot(&d, &ray->n))
					* 2,
					&c);
		}
		else
		{
			/* Specular reflection */
			vec3_t c;
			vec3_t d;

			vec3_set(&c, &vec3_zero);
			vec3_fma(&d, &ray->d, -2 * n_cos, &ray->n);

			ray_trace(&c, &ray->q, &d, ray);

			vec3_fma(&ray->c, &ray->c, ref_rc, &c);
		}
	}
	else
	{
		/* Transmission */
		vec3_t c;
		vec3_t d;

		vec3_set(&c, &vec3_zero);

		vec3_scale(&d, n_cos, &ray->n);
		vec3_sub(&d, &ray->d, &d);
		vec3_scale(&d, 1 / ind, &d);
		n_sin_sq = vec3_len_sq(&d);

		vec3_fma(&d, &d,
			copysign(sqrt(1 - n_sin_sq), n_cos),
			&ray->n);

		ray_trace(&c, &ray->q, &d, ray);

		vec3_fma(&ray->c, &ray->c, tra_rc, &c);
	}

	/* Ambient */
	{
		vec3_fma(&ray->c, &ray->c, mat->amb, &vec3_unit);
	}

	/* Texture */
	{
		vec3_nmul(&ray->c, &ray->c, &tc);
	}

#if 0
	/* Attenuation */
	if ((mat->flg & MAT_FLAT) == 0)
	{
		real_t m = 100;
		real_t a = m / vec3_len_sq(&r);

		if (a > 1)
		{
			a = 1;
		}

		vec3_scale(&ray->c, a, &ray->c);
	}
#endif
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
		tri_t *tri = &scene.p_tri[i];

		tri_trace(tri, &ray);
	}

	for (int i = 0; i < scene.n_sph; i++)
	{
		sph_t *sph = &scene.p_sph[i];

		sph_trace(sph, &ray);
	}

	for (int i = 0; i < scene.n_sph_light; i++)
	{
		sph_t *sph = &scene.p_sph_light[i];

		ray_sph_light(sph, c, p, d, &ray.l, ray.depth + 1);
	}

	if (ray.mat != NULL)
	{
		ray_shade(&ray);
		vec3_add(c, c, &ray.c);
	}

#if 1
	{
		/* Directional light */
		real_t lm	= 0.5;
		vec3_t lc	= { 1.00, 1.00, 0.95 };
		vec3_t ln	= {  0.1, -1.0,  0.1 };

		vec3_norm(&ln, &ln);

		real_t a = vec3_dot(&ray.d, &ln);

		if (a < 0)
		{
			vec3_fma(c, c, -a * lm, &lc);
		}
	}
#endif

	return 0;
}