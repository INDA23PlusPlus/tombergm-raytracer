#include "sph.h"
#include "ray.h"
#include "vec.h"

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

	if (s <= 0)
	{
		/* Ray is moving away */
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
	else if (s >= ray->l)
	{
		/* Sphere is occluded */
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

real_t sph_trace2(	const sph_t *sph, vec3_t *p, vec3_t *d,
			real_t m, void *prev)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t h;
	real_t l;
	real_t a;

	o = sph->c - *p;
	l = dot(o, *d);

	if (l <= 0)
	{
		/* Ray is moving away */
		return INFINITY;
	}

	v = l * *d;
	w = v - o;
	h = length(w);

	if (h > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return INFINITY;
	}

	a = 1 - (sph->r - h) / sph->r;
	a = sqrt(1 - a * a);

	if (sph == prev)
	{
		/* Hit the back surface with transmission rays */
		l = l + sph->r * a;
	}
	else
	{
		/* Otherwise hit the front surface */
		l = l - sph->r * a;
	}

	if (0 >= l || l >= m)
	{
		return INFINITY;
	}

	return l;
}

void sph_hit(const sph_t *sph, ray_t *ray)
{
	ray->n = ray->q - sph->c;
	ray->n = ray->n / sph->r;

	ray->mat = sph->mat;

	if (mat_has_tex(ray->mat))
	{
		ray->uv.x = 0.5 + asin(ray->n.x) / M_PI;
		ray->uv.y = 0.5 + asin(ray->n.z) / M_PI;
	}

}
