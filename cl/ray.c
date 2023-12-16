#include "mat.h"
#include "ray.h"
#include "scene.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"
#include "vec.h"

#define RAY_DEPTH 8

static inline
void ray_trace_f0(	__constant scene_t *scene, ray_t *ray, ray_t *rec,
			unsigned *rand)
{
	float n_cos;
	float n_sin_sq;

	ray->d		= normalize(ray->d);
	ray->curr	= NULL;
	ray->l		= 1. / 0.;
	ray->mat	= NULL;
	ray->c		= 0;

	for (int i = 0; i < scene->n_tri; i++)
	{
		tri_t *tri = &scene->p_tri[i];

		tri_trace(tri, ray);
	}

	for (int i = 0; i < scene->n_sph; i++)
	{
		sph_t *sph = &scene->p_sph[i];

		sph_trace(sph, ray);
	}

	if (ray->mat == NULL || rec == NULL)
	{
		/* No recursive rays */
		return;
	}

	ray->tc = 1;
	ray->tr = 1;

	if (mat_has_tex(ray->mat))
	{
		vec3_t tn;

		tex_sample(	ray->mat->tex, &ray->uv,
				&ray->tc, &tn, &ray->tr);

		if (tex_has_n(ray->mat->tex))
		{
			ray->n = tn.z * ray->n;
			ray->n = ray->n + tn.x * ray->tu;
			ray->n = ray->n + tn.y * ray->tv;
			ray->n = normalize(ray->n);
		}
	}

	ray->tc = ray->tc * ray->mat->col;

	if ((ray->mat->flg & MAT_FLAT) != 0)
	{
		return;
	}

	n_cos = dot(ray->d, ray->n);

	float ref_rc = ray->mat->ref;
	float tra_rc = ray->tr;
	float ind;
	float R;

	if (n_cos < 0)
		ind = ray->mat->ind;
	else
		ind = 1. / ray->mat->ind;

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

	if (flt_rand(rand) * ray->mat->tra <= R)
	{
		if (flt_rand(rand) * ray->tr < ray->mat->dif)
		{
			/* Diffuse reflection */
			rec->prev = ray->curr;
			rec->p = ray->q;
			vec3_diffuse(&rec->d, &ray->n, M_PI / 2, rand);
			ray->rc = ref_rc * fabs(dot(rec->d, ray->n)) * 2;
		}
		else
		{
			/* Specular reflection */
			rec->prev = ray->curr;
			rec->p = ray->q;
			rec->d = ray->d - 2 * n_cos * ray->n;
			ray->rc = ref_rc;
		}
	}
	else
	{
		/* Transmission */
		rec->prev = ray->curr;
		rec->p = ray->q;
		rec->d = (ray->d - n_cos * ray->n) / ind;
		n_sin_sq = dot(rec->d, rec->d);
		rec->d = rec->d + copysign(sqrt(1 - n_sin_sq), n_cos) * ray->n;
		ray->rc = tra_rc;
	}
}

static inline
void ray_shade_f0(__constant scene_t *scene, ray_t *ray, ray_t *rec)
{
	const mat_t *mat = ray->mat;

	/* Recursive ray */
	if (rec != NULL && rec->curr != NULL)
	{
		ray->c = ray->c + ray->rc * rec->c;
	}

	/* Ambient */
	{
		ray->c = ray->c + mat->amb;
	}

	/* Texture */
	{
		ray->c = ray->c * ray->tc;
	}

#if 1
	{
		/* Directional light */
		real_t lm	= 0.5;
		vec3_t lc	= { 1.00, 1.00, 0.95 };
		vec3_t ln	= {  0.1, -1.0,  0.1 };

		ln = normalize(ln);

		real_t a = dot(ray->d, ln);

		if (a < 0)
		{
			ray->c = ray->c - a * lm * lc;
		}
	}
#endif

	for (int i = 0; i < scene->n_sph_light; i++)
	{
		sph_t *sph = &scene->p_sph_light[i];

		ray_sph_light(sph, &ray->c, &ray->p, &ray->d, &ray->l);
	}

#if 0
	/* Attenuation */
	if ((mat->flg & MAT_FLAT) == 0)
	{
		vec3_t r = ray->q - ray->p;
		real_t m = 100;
		real_t a = m / dot(r, r);

		if (a > 1)
		{
			a = 1;
		}

		ray->c = a * ray->c;
	}
#endif
}

static inline
void ray_trace(	__constant scene_t *scene,
		vec3_t *c, vec3_t *p, vec3_t *d,
		unsigned *rand)
{
	ray_t	rays[RAY_DEPTH];
	int	i;
	int	j;

	for (i = 0; i < RAY_DEPTH; i++)
	{
		rays[i].curr = NULL;
		rays[i].prev = NULL;
	}

	rays[0].prev = &rays[0];
	rays[0].p = *p;
	rays[0].d = *d;

	i = 0;
	j = 1;

	while (i < RAY_DEPTH)
	{
		if (rays[i].prev != NULL)
		{
			if (j < RAY_DEPTH)
			{
				ray_trace_f0(scene, &rays[i], &rays[j], rand);
			}
			else
			{
				ray_trace_f0(scene, &rays[i], NULL, rand);
			}
		}

		i = i + 1;
		j = j + 1;
	}

	do
	{
		i = i - 1;
		j = j - 1;

		if (rays[i].curr != NULL)
		{
			if (j < RAY_DEPTH)
			{
				ray_shade_f0(scene, &rays[i], &rays[j]);
			}
			else
			{
				ray_shade_f0(scene, &rays[i], NULL);
			}
		}
	}
	while (i > 0);

	*c = rays[0].c;
}
