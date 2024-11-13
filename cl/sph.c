#include "mat.h"
#include "scene.h"
#include "sph.h"
#include "ray.h"
#include "vec.h"

real_t sph_trace(const sph_t *sph, ray_t *ray, bool prev)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t h;
	real_t l;
	real_t a;

	o = sph->c - ray->p;
	l = dot(o, ray->d);

	if (l <= 0)
	{
		/* Ray is moving away */
		return INFINITY;
	}

	v = l * ray->d;
	w = v - o;
	h = length(w);

	if (h > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return INFINITY;
	}

	a = 1 - (sph->r - h) / sph->r;
	a = sqrt(1 - a * a);

	if (prev)
	{
		/* Hit the back surface with transmission rays */
		l = l + sph->r * a;
	}
	else
	{
		/* Otherwise hit the front surface */
		l = l - sph->r * a;
	}

	if (0 >= l || l >= ray->l)
	{
		return INFINITY;
	}

	return l;
}

void sph_hit(SCENE, const sph_t *sph, ray_t *ray)
{
	ray->n = ray->q - sph->c;
	ray->n = ray->n / sph->r;

	ray->mat = sph->mat;

	if (mat_has_tex(MAT(ray->mat)))
	{
		ray->uv.x = 0.5 + asin(ray->n.x) / M_PI;
		ray->uv.y = 0.5 + asin(ray->n.z) / M_PI;
	}
}
