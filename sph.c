#include <tgmath.h>
#include "box.h"
#include "mat.h"
#include "sph.h"
#include "ray.h"
#include "vec.h"

void sph_get_box(const sph_t *sph, box_t *box)
{
	box->min[0] = sph->c.x - sph->r;
	box->max[0] = sph->c.x + sph->r;
	box->min[1] = sph->c.y - sph->r;
	box->max[1] = sph->c.y + sph->r;
	box->min[2] = sph->c.z - sph->r;
	box->max[2] = sph->c.z + sph->r;
}

real_t sph_trace(	const sph_t *sph, vec3_t *p, vec3_t *d,
			real_t m, void *prev)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t h;
	real_t l;
	real_t a;

	vec3_sub(&o, &sph->c, p);
	l = vec3_dot(&o, d);

	if (l <= 0)
	{
		/* Ray is moving away */
		return INFINITY;
	}

	vec3_scale(&v, l, d);
	vec3_sub(&w, &v, &o);
	h = vec3_len(&w);

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
	vec3_sub(&ray->n, &ray->q, &sph->c);
	vec3_scale(&ray->n, 1 / sph->r, &ray->n);

	ray->mat = sph->mat;

	if (mat_has_tex(ray->mat))
	{
		ray->uv.x = 0.5 + asin(ray->n.x) / M_PI;
		ray->uv.y = 0.5 + asin(ray->n.z) / M_PI;
	}

}
