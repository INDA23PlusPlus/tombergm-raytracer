#include "bih.h"
#include "mat.h"
#include "prim.h"
#include "ray.h"
#include "scene.h"
#include "tex.h"
#include "vec.h"

#define RAY_DEPTH 8

static void trace_reg(SCENE, ray_t *ray, ray_t *rec, unsigned *rand)
{
	real_t n_sin_sq;
	real_t n_cos	= dot(ray->d, ray->n);
	real_t ref_rc	= MAT(ray->mat)->ref;
	real_t tra_rc	= ray->tr;
	real_t ind;
	real_t R;

	if (n_cos < 0)
	{
		ind = MAT(ray->mat)->ind;
	}
	else
	{
		ind = 1. / MAT(ray->mat)->ind;
	}

	{
		/* Fresnel equation for non-magnetic reflectivity */
		real_t b = ind * ind + n_cos * n_cos - 1;

		if (b < 0)
		{
			/* Total internal reflection */
			R = INFINITY;
			ref_rc = 1;
		}
		else
		{
			b = sqrt(b);
			R = (fabs(n_cos) - b) / (fabs(n_cos) + b);
			R = R * R;
		}
	}

	if (flt_rand(rand) * MAT(ray->mat)->tra <= R)
	{
		if (flt_rand(rand) * ray->tr < MAT(ray->mat)->dif)
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

static void trace_bsdf(SCENE, ray_t *ray, ray_t *rec, unsigned *rand)
{
	real_t n_cos;
	real_t r = MAT(ray->mat)->dif * ray->tr;
	real_t ind;

	r = min2(0.01f, r);

	vec3_t N = ray->n;
	vec3_t V = -ray->d;
	vec3_t M;

	{
		/* Microfacet, GGX */
		real_t a = flt_rand(rand) * M_PI * 2;
		real_t k = flt_rand(rand);
		real_t c = sqrt(max(0.f, (1 - k) / (1 + (r * r - 1) * k)));
		real_t s = sqrt(max(0.f, 1 - c * c));
		vec3_t u;
		vec3_t v;

		vec3_perp(&u, &N);
		v = cross(u, N);

		M = cos(a) * s * u + sin(a) * s * v + N * c;
	}

	vec3_t L = normalize(2.f * dot(V, M) * M - V);

	real_t NdotV = dot(N, V);
	real_t NdotL = dot(N, L);
	real_t NdotM = dot(N, M);
	real_t LdotM = dot(L, M);

	if (NdotV < 0 || NdotL < 0 || NdotM < 0 || LdotM < 0)
	{
		return;
	}

	real_t G;
	real_t F;

	{
		/* Geometric masking term, Schlick / Karis */
		real_t k = r * r / 2;

		G =	(NdotV * (NdotL * (1 - k) + k))	/
			(NdotL * (NdotV * (1 - k) + k))	;
	}

	if (n_cos < 0)
	{
		ind = MAT(ray->mat)->ind;
	}
	else
	{
		ind = 1. / MAT(ray->mat)->ind;
	}

	{
#if 0
		/* Fresnel */
		real_t b = ind * ind + LdotM * LdotM - 1;

		if (b < 0)
		{
			/* Total internal reflection */
			F = 0;
		}
		else
		{
			b = sqrt(b);
			F = (LdotM - b) / (LdotM + b);
			F = F * F;
		}
#else
		real_t n = 0.239;
		real_t k = 3.416;
		real_t i = n * n - k * k - (1 - LdotM * LdotM);
		real_t j = sqrt(i * i + 4 * n * n * k * k);
		real_t a = i + j;
		real_t b = i - j;
		i = (a + b) / 2 + LdotM * LdotM;
		j = sqrt(a) * LdotM;
		F = (i - j) / (i + j);
#endif
	}

	{
		/* Reflection */
		rec->prev = ray->curr;
		rec->p = ray->q;
		rec->d = L;

		ray->rc = G * F * LdotM / (NdotV * NdotM);
	}
}

static void trace(SCENE, ray_t *ray, ray_t *rec, unsigned *rand)
{
	ray->d = normalize(ray->d);
	ray->l = INFINITY;

	bih_trace(scene, ray);

	if (ray->curr == PRIM_NULL)
	{
		/* Nothing hit */
		return;
	}

	ray->q = ray->p + ray->l * ray->d;

	prim_hit(scene, PRIM(ray->curr), ray);

	ray->c = 0;
	ray->tc = 1;
	ray->tr = 1;

	if (mat_has_tex(MAT(ray->mat)))
	{
		vec3_t tn;

		tex_sample(	scene, TEX(MAT(ray->mat)->tex), &ray->uv,
				&ray->tc, &tn, &ray->tr);

		if (tex_has_n(TEX(MAT(ray->mat)->tex)))
		{
			ray->n = tn.z * ray->n;
			ray->n = ray->n + tn.x * ray->tu;
			ray->n = ray->n + tn.y * ray->tv;
			ray->n = normalize(ray->n);
		}
	}

	ray->tc = ray->tc * MAT(ray->mat)->col;

	if ((MAT(ray->mat)->flg & MAT_FLAT) != 0 || rec->curr != PRIM_NULL)
	{
		/* No recursive rays */
		return;
	}

	switch (MAT(ray->mat)->sha)
	{
		case 0	: trace_reg	(scene, ray, rec, rand);	break;
		case 1	: trace_bsdf	(scene, ray, rec, rand);	break;
		default	:						break;
	}
}

static void shade(SCENE, ray_t *ray, ray_t *rec)
{
	const mat_t *mat = MAT(ray->mat);

	/* Recursive ray */
	if (rec->prev != PRIM_NULL)
	{
		ray->c = ray->c + ray->rc * rec->c;
	}

	/* Ambient */
	if ((mat->flg & MAT_FLAT) == 0)
	{
		ray->c = ray->c + mat->amb * fabs(dot(ray->d, ray->n));
	}
	else
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
}

void ray_trace(SCENE, vec3_t *c, vec3_t *p, vec3_t *d, unsigned *rand)
{
	ray_t	no_ray;
	ray_t	rays[RAY_DEPTH];
	int	i;
	int	j;

	no_ray.prev = PRIM_NULL;
	no_ray.curr = 0;

	for (i = 0; i < RAY_DEPTH; i++)
	{
		rays[i].curr = PRIM_NULL;
		rays[i].prev = PRIM_NULL;
	}

	rays[0].prev	= scene_n_prim;
	rays[0].p	= *p;
	rays[0].d	= *d;

	i = 0;
	j = 1;

	while (i < RAY_DEPTH)
	{
		if (rays[i].prev != PRIM_NULL)
		{
			if (j < RAY_DEPTH)
			{
				trace(scene, &rays[i], &rays[j], rand);
			}
			else
			{
				trace(scene, &rays[i], &no_ray, rand);
			}
		}

		i = i + 1;
		j = j + 1;
	}

	do
	{
		i = i - 1;
		j = j - 1;

		if (rays[i].curr != PRIM_NULL)
		{
			if (j < RAY_DEPTH)
			{
				shade(scene, &rays[i], &rays[j]);
			}
			else
			{
				shade(scene, &rays[i], &no_ray);
			}
		}
	}
	while (i > 0);

	if (rays[0].curr != PRIM_NULL)
	{
		*c = rays[0].c;
	}
}
