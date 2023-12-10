#ifndef VEC_H
#define VEC_H

typedef double	real_t;

typedef struct
{
	real_t	x;
	real_t	y;
	real_t	z;
} vec3_t;

extern const vec3_t vec3_zero;
extern const vec3_t vec3_unit;

void	vec3_set(vec3_t *r, const vec3_t *a);
void	vec3_add(vec3_t *r, const vec3_t *a, const vec3_t *b);
void	vec3_sub(vec3_t *r, const vec3_t *a, const vec3_t *b);
void	vec3_clamp(vec3_t *r, const vec3_t *a,
			const vec3_t *lo, const vec3_t *hi);
void	vec3_scale(vec3_t *r, real_t s, const vec3_t *a);
void	vec3_fma(vec3_t *r, const vec3_t *a, real_t s, const vec3_t *b);
void	vec3_nmul(vec3_t *r, const vec3_t *a, const vec3_t *b);
real_t	vec3_len(const vec3_t *a);
real_t	vec3_len_sq(const vec3_t *a);
void	vec3_norm(vec3_t *r, const vec3_t *a);
real_t	vec3_dot(const vec3_t *a, const vec3_t *b);
real_t	vec3_cos(const vec3_t *a, const vec3_t *b);
real_t	vec3_proj_s(const vec3_t *a, const vec3_t *b);
void	vec3_proj_v(vec3_t *r, const vec3_t *a, const vec3_t *b);
void	vec3_rej_v(vec3_t *r, const vec3_t *a, const vec3_t *b);
void	vec3_cross(vec3_t *r, const vec3_t *a, const vec3_t *b);
void	vec3_diffuse(vec3_t *r, const vec3_t *n, real_t a);

typedef struct
{
	real_t	x;
	real_t	y;
} vec2_t;

extern const vec2_t vec2_zero;
extern const vec2_t vec2_unit;

void	vec2_set(vec2_t *r, const vec2_t *a);
void	vec2_add(vec2_t *r, const vec2_t *a, const vec2_t *b);
void	vec2_sub(vec2_t *r, const vec2_t *a, const vec2_t *b);
void	vec2_clamp(vec2_t *r, const vec2_t *a,
			const vec2_t *lo, const vec2_t *hi);
void	vec2_scale(vec2_t *r, real_t s, const vec2_t *a);
void	vec2_fma(vec2_t *r, const vec2_t *a, real_t s, const vec2_t *b);
void	vec2_nmul(vec2_t *r, const vec2_t *a, const vec2_t *b);
real_t	vec2_len(const vec2_t *a);
real_t	vec2_len_sq(const vec2_t *a);
void	vec2_norm(vec2_t *r, const vec2_t *a);
real_t	vec2_dot(const vec2_t *a, const vec2_t *b);
real_t	vec2_cos(const vec2_t *a, const vec2_t *b);
real_t	vec2_proj_s(const vec2_t *a, const vec2_t *b);
void	vec2_proj_v(vec2_t *r, const vec2_t *a, const vec2_t *b);
void	vec2_rej_v(vec2_t *r, const vec2_t *a, const vec2_t *b);

extern unsigned rng_val;

static inline real_t flt_rand(void)
{
	rng_val = rng_val * 1664525 + 1013904223;

	return (float) rng_val / (float) 0xFFFFFFFF;
}

#endif
