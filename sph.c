#include <math.h>
#include "sph.h"
#include "ray.h"
#include "vec.h"

int sph_trace(sph_t *sph, ray_t *ray)
{
	vec_t o;
	vec_t v;
	vec_t w;
	float s;
	float t;
	float a;

	vec_sub(&o, &sph->c, &ray->p);
	s = vec_dot(&o, &ray->d);

	if (s >= ray->l)
	{
		/* Sphere is occluded */
		return 0;
	}

	if (s <= 0)
	{
		/* Ray moving away */
		return 0;
	}

	vec_scale(&v, s, &ray->d);
	vec_sub(&w, &v, &o);
	t = vec_len(&w);

	if (t > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return 0;
	}

	a = 1 - (sph->r - t) / sph->r;
	a = sqrtf(1 - a * a);

	if (ray->prev == sph)
	{
		/* Hit the back surface with transmission rays */
		s = s + sph->r * a;
	}
	else
	{
		/* Otherwise hit the front surface */
		s = s - sph->r * a;
	}

	if (s <= 0)
	{
		/* Sphere is behind ray origin */
		return 0;
	}

	{
		ray->curr = sph;

		vec_fma(&ray->q, &ray->p, s, &ray->d);
		vec_sub(&ray->n, &ray->q, &sph->c);
		vec_scale(&ray->n, 1 / sph->r, &ray->n);
		ray->l = s;

		ray->mat = sph->mat;

		return 1;
	}
}

int ray_sph_light(sph_t *sph, vec_t *c, vec_t *p, vec_t *d,
			float *dist, int depth)
{
	vec_t o;
	vec_t v;
	vec_t w;
	float s;
	float t;
	float a;

	vec_sub(&o, &sph->c, p);
	s = vec_dot(&o, d);

	if (s >= *dist)
	{
		/* Sphere is occluded */
		return 0;
	}

	vec_scale(&v, s, d);
	vec_sub(&w, &v, &o);
	t = vec_len(&w);

	if (t > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return 0;
	}

	a = 1 - (sph->r - t) / sph->r;
	a = sqrtf(1 - a * a);
	s = s - sph->r * a;

	if (s <= 0)
	{
		/* Sphere is behind ray origin */
		return 0;
	}

	{
		float m = 100;

		a = m * 2 * a * a * sph->r / (s * s);

		vec_fma(c, c, a, &vec_unit);

		return 1;
	}
}
