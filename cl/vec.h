#ifndef VEC_H
#define VEC_H

typedef float	real_t;
typedef float2	vec2_t;
typedef float3	vec3_t;

#define min2(a, b)	fmin(a, b)
#define min3(a, b, c)	min2(min2(a, b), c)
#define max2(a, b)	fmax(a, b)
#define max3(a, b, c)	max2(a, max2(b, c))

void	vec3_diffuse(vec3_t *r, const vec3_t *n, real_t a, unsigned *rand);
void	vec3_perp(vec3_t *r, const vec3_t *a);

static inline real_t flt_rand(unsigned *rand)
{
	*rand = *rand * 1664525 + 1013904223;

	return (float) *rand / (float) 0xFFFFFFFF;
}

#endif
