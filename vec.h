#ifndef VEC_H
#define VEC_H

typedef struct
{
	float	x;
	float	y;
	float	z;
} vec_t;

typedef struct
{
	float	x;
	float	y;
} vec2_t;

extern const vec_t vec_zero;
extern const vec_t vec_unit;

void	vec_cross(vec_t *r, const vec_t *a, const vec_t *b);
float	vec_dot(const vec_t *a, const vec_t *b);
void	vec_scale(vec_t *r, float s, const vec_t *a);
float	vec_len_sq(const vec_t *a);
float	vec_len(const vec_t *a);
void	vec_norm(vec_t *r, const vec_t *a);
void	vec_add(vec_t *r, const vec_t *a, const vec_t *b);
void	vec_set(vec_t *r, const vec_t *a);
void	vec_fma(vec_t *r, const vec_t *a, float s, const vec_t *b);
void	vec_sub(vec_t *r, const vec_t *a, const vec_t *b);
void	vec_nmul(vec_t *r, const vec_t *a, const vec_t *b);
void	vec_clamp(vec_t *r, const vec_t *a, const vec_t *lo, const vec_t *hi);
float	vec_cos(const vec_t *a, const vec_t *b);
float	vec_proj_s(const vec_t *a, const vec_t *b);
void	vec_proj_v(vec_t *r, const vec_t *a, const vec_t *b);
void	vec_rej_v(vec_t *r, const vec_t *a, const vec_t *b);

void	vec2_sub(vec2_t *r, const vec2_t *a, const vec2_t *b);

#endif
