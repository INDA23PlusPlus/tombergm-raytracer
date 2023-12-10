#ifndef VEC_H
#define VEC_H

typedef float	real_t;
typedef float2	vec2_t;
typedef float3	vec3_t;

static inline
void vec3_diffuse(vec3_t *r, const vec3_t *n, real_t a, unsigned *rand);

static inline real_t flt_rand(unsigned *rand)
{
	*rand = *rand * 1664525 + 1013904223;

	return (float) *rand / (float) 0xFFFFFFFF;
}

#endif
