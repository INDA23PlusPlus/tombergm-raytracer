#include "box.h"
#include "vec.h"

real_t box_trace(const box_t *box, vec3_t *p, vec3_t *d, real_t m)
{
	real_t l_ax = (box->min[0] - p->x) / d->x;
	real_t l_bx = (box->max[0] - p->x) / d->x;
	real_t l_ay = (box->min[1] - p->y) / d->y;
	real_t l_by = (box->max[1] - p->y) / d->y;
	real_t l_az = (box->min[2] - p->z) / d->z;
	real_t l_bz = (box->max[2] - p->z) / d->z;

	real_t l_f = max3(min2(l_ax, l_bx), min2(l_ay, l_by), min2(l_az, l_bz));
	real_t l_b = min3(max2(l_ax, l_bx), max2(l_ay, l_by), max2(l_az, l_bz));

	if (0 > l_b || l_f > min2(l_b, m))
	{
		return INFINITY;
	}

	return l_f;
}
