#include <tgmath.h>
#include "sph.h"
#include "ray.h"
#include "vec.h"

int sph_trace(sph_t *sph, ray_t *ray)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t s;
	real_t t;
	real_t a;

	vec3_sub(&o, &sph->c, &ray->p);
	s = vec3_dot(&o, &ray->d);

	if (s <= 0)
	{
		/* Ray is moving away */
		return 0;
	}

	vec3_scale(&v, s, &ray->d);
	vec3_sub(&w, &v, &o);
	t = vec3_len(&w);

	if (t > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return 0;
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
		return 0;
	}
	else if (s >= ray->l)
	{
		/* Sphere is occluded */
		return 0;
	}

	{
		ray->curr = sph;

		vec3_fma(&ray->q, &ray->p, s, &ray->d);
		vec3_sub(&ray->n, &ray->q, &sph->c);
		vec3_scale(&ray->n, 1 / sph->r, &ray->n);
		ray->l = s;

		ray->mat = sph->mat;
		if (mat_has_tex(ray->mat))
		{
			ray->uv.x = 0.5 + asin(ray->n.x) / M_PI;
			ray->uv.y = 0.5 + asin(ray->n.z) / M_PI;
		}

		return 1;
	}
}
