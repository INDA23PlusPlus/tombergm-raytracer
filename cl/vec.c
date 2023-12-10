#include "vec.h"

static inline
void vec3_diffuse(vec3_t *r, const vec3_t *n, real_t a, unsigned *rand)
{
	vec3_t m;

#if 0
	{
		real_t p = acos(2 * flt_rand(rand) - 1) - M_PI / 2;
		real_t t = 2 * M_PI * flt_rand(rand);

		m.x = cos(p) * cos(t);
		m.y = cos(p) * sin(t);
		m.z = sin(p);

		m = cross(m, *n);
	}
#else
	m.x = 1 - 2 * flt_rand(rand);
	m.y = 1 - 2 * flt_rand(rand);
	m.z = 1 - 2 * flt_rand(rand);

	m = normalize(cross(m, *n));
#endif

	a = a * flt_rand(rand);

	*r = cos(a) * *n;
	*r = *r + sin(a) * m;
}
