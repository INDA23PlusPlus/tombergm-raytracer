#include "sph.h"
#include "ray.h"
#include "vec.h"

static inline
void sph_trace(sph_t *sph, ray_t *ray)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t s;
	real_t t;
	real_t a;

	o = sph->c - ray->p;
	s = dot(o, ray->d);

	if (s >= ray->l)
	{
		/* Sphere is occluded */
		return;
	}

	if (s <= 0)
	{
		/* Ray moving away */
		return;
	}

	v = s * ray->d;
	w = v - o;
	t = length(w);

	if (t > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return;
	}

	a = 1 - (sph->r - t) / sph->r;
	a = sqrt(1 - a * a);

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
		return;
	}

	{
		ray->curr = sph;

		ray->q = ray->p + s * ray->d;
		ray->n = ray->q - sph->c;
		ray->n = ray->n / sph->r;
		ray->l = s;

		ray->mat = sph->mat;
		if (mat_has_tex(ray->mat))
		{
			ray->uv.x = 0.5 + asin(ray->n.x) / M_PI;
			ray->uv.y = 0.5 + asin(ray->n.z) / M_PI;
		}
	}
}

static inline
void ray_sph_light(	sph_t *sph, vec3_t *c, vec3_t *p, vec3_t *d,
			real_t *dist)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t s;
	real_t t;
	real_t a;

	o = sph->c - *p;
	s = dot(o, *d);

	if (s >= *dist)
	{
		/* Sphere is occluded */
		return;
	}

	v = s * *d;
	w = v - o;
	t = length(w);

	if (t > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return;
	}

	a = 1 - (sph->r - t) / sph->r;
	a = sqrt(1 - a * a);
	s = s - sph->r * a;

	if (s <= 0)
	{
		/* Sphere is behind ray origin */
		return;
	}

	{
		real_t m = 50;

		a = m * 2 * a * a * sph->r;

		*c = *c + a;
	}
}
