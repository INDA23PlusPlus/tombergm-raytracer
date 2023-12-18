#include <tgmath.h>
#include "vec.h"

const vec2_t vec2_zero = { 0, 0 };
const vec2_t vec2_unit = { 1, 1 };

void vec2_set(vec2_t *r, const vec2_t *a)
{
	*r = *a;
}

void vec2_add(vec2_t *r, const vec2_t *a, const vec2_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
}

void vec2_sub(vec2_t *r, const vec2_t *a, const vec2_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
}

void vec2_clamp(vec2_t *r, const vec2_t *a, const vec2_t *lo, const vec2_t *hi)
{
	r->x = (a->x > lo->x ? a->x < hi->x ? a->x : hi->x : lo->x);
	r->y = (a->y > lo->y ? a->y < hi->y ? a->y : hi->y : lo->y);
}

void vec2_scale(vec2_t *r, real_t s, const vec2_t *a)
{
	r->x = s * a->x;
	r->y = s * a->y;
}

void vec2_fma(vec2_t *r, const vec2_t *a, real_t s, const vec2_t *b)
{
	r->x = a->x + s * b->x;
	r->y = a->y + s * b->y;
}

void vec2_nmul(vec2_t *r, const vec2_t *a, const vec2_t *b)
{
	r->x = a->x * b->x;
	r->y = a->y * b->y;
}

real_t vec2_len(const vec2_t *a)
{
	return sqrt(vec2_len_sq(a));
}

real_t vec2_len_sq(const vec2_t *a)
{
	return vec2_dot(a, a);
}

void vec2_norm(vec2_t *r, const vec2_t *a)
{
	vec2_scale(r, 1 / vec2_len(a), a);
}

real_t vec2_dot(const vec2_t *a, const vec2_t *b)
{
	return a->x * b->x + a->y * b->y;
}

real_t vec2_cos(const vec2_t *a, const vec2_t *b)
{
	return vec2_dot(a, b) / (vec2_len(a) * vec2_len(b));
}

real_t vec2_proj_s(const vec2_t *a, const vec2_t *b)
{
	return vec2_dot(a, b) / vec2_len(b);
}

void vec2_proj_v(vec2_t *r, const vec2_t *a, const vec2_t *b)
{
	vec2_scale(r, vec2_dot(a, b) / vec2_len_sq(b), b);
}

void vec2_rej_v(vec2_t *r, const vec2_t *a, const vec2_t *b)
{
	vec2_t v;

	vec2_proj_v(&v, a, b);
	vec2_sub(r, a, &v);
}

const vec3_t vec3_zero = { 0, 0, 0 };
const vec3_t vec3_unit = { 1, 1, 1 };

void vec3_set(vec3_t *r, const vec3_t *a)
{
	*r = *a;
}

void vec3_add(vec3_t *r, const vec3_t *a, const vec3_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

void vec3_sub(vec3_t *r, const vec3_t *a, const vec3_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
}

void vec3_clamp(vec3_t *r, const vec3_t *a, const vec3_t *lo, const vec3_t *hi)
{
	r->x = (a->x > lo->x ? a->x < hi->x ? a->x : hi->x : lo->x);
	r->y = (a->y > lo->y ? a->y < hi->y ? a->y : hi->y : lo->y);
	r->z = (a->z > lo->z ? a->z < hi->z ? a->z : hi->z : lo->z);
}

void vec3_scale(vec3_t *r, real_t s, const vec3_t *a)
{
	r->x = s * a->x;
	r->y = s * a->y;
	r->z = s * a->z;
}

void vec3_fma(vec3_t *r, const vec3_t *a, real_t s, const vec3_t *b)
{
	r->x = a->x + s * b->x;
	r->y = a->y + s * b->y;
	r->z = a->z + s * b->z;
}

void vec3_nmul(vec3_t *r, const vec3_t *a, const vec3_t *b)
{
	r->x = a->x * b->x;
	r->y = a->y * b->y;
	r->z = a->z * b->z;
}

real_t vec3_len(const vec3_t *a)
{
	return sqrt(vec3_len_sq(a));
}

real_t vec3_len_sq(const vec3_t *a)
{
	return vec3_dot(a, a);
}

void vec3_norm(vec3_t *r, const vec3_t *a)
{
	vec3_scale(r, 1 / vec3_len(a), a);
}

real_t vec3_dot(const vec3_t *a, const vec3_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

real_t vec3_cos(const vec3_t *a, const vec3_t *b)
{
	return vec3_dot(a, b) / (vec3_len(a) * vec3_len(b));
}

real_t vec3_proj_s(const vec3_t *a, const vec3_t *b)
{
	return vec3_dot(a, b) / vec3_len(b);
}

void vec3_proj_v(vec3_t *r, const vec3_t *a, const vec3_t *b)
{
	vec3_scale(r, vec3_dot(a, b) / vec3_len_sq(b), b);
}

void vec3_rej_v(vec3_t *r, const vec3_t *a, const vec3_t *b)
{
	vec3_t v;

	vec3_proj_v(&v, a, b);
	vec3_sub(r, a, &v);
}

void vec3_cross(vec3_t *r, const vec3_t *a, const vec3_t *b)
{
	vec3_t v;

	v.x = a->y * b->z - a->z * b-> y;
	v.y = a->z * b->x - a->x * b-> z;
	v.z = a->x * b->y - a->y * b-> x;

	*r = v;
}

void vec3_diffuse(vec3_t *r, const vec3_t *n, real_t a)
{
	vec3_t m;

#if 0
	{
		real_t p = acos(2 * flt_rand() - 1) - M_PI / 2;
		real_t t = 2 * M_PI * flt_rand();

		m.x = cos(p) * cos(t);
		m.y = cos(p) * sin(t);
		m.z = sin(p);
	}
#else
	m.x = 1 - 2 * flt_rand();
	m.y = 1 - 2 * flt_rand();
	m.z = 1 - 2 * flt_rand();
#endif

	vec3_cross(&m, &m, n);
	vec3_norm(&m, &m);

	a = a * flt_rand();

	vec3_scale(r, cos(a), n);
	vec3_fma(r, r, sin(a), &m);
}

unsigned rng_val;
