#include "box.h"
#include "ray.h"
#include "vec.h"

real_t box_trace(__constant const box_t *box, ray_t *ray)
{
	real_t l_ax = (box->min[0] - ray->p.x) / ray->d.x;
	real_t l_bx = (box->max[0] - ray->p.x) / ray->d.x;
	real_t l_ay = (box->min[1] - ray->p.y) / ray->d.y;
	real_t l_by = (box->max[1] - ray->p.y) / ray->d.y;
	real_t l_az = (box->min[2] - ray->p.z) / ray->d.z;
	real_t l_bz = (box->max[2] - ray->p.z) / ray->d.z;

	real_t l_f = max3(min2(l_ax, l_bx), min2(l_ay, l_by), min2(l_az, l_bz));
	real_t l_b = min3(max2(l_ax, l_bx), max2(l_ay, l_by), max2(l_az, l_bz));

	if (0 > l_b || l_f > min2(l_b, ray->l))
	{
		return INFINITY;
	}

	return l_f;
}
