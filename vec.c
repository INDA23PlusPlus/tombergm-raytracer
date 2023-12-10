#include <math.h>
#include "vec.h"

const vec_t vec_zero = { 0, 0, 0 };
const vec_t vec_unit = { 1, 1, 1 };

void vec_cross(vec_t *r, const vec_t *a, const vec_t *b)
{
	vec_t v;

	v.x = a->y * b->z - a->z * b-> y;
	v.y = a->z * b->x - a->x * b-> z;
	v.z = a->x * b->y - a->y * b-> x;

	*r = v;
}

float vec_dot(const vec_t *a, const vec_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vec_scale(vec_t *r, float s, const vec_t *a)
{
	r->x = s * a->x;
	r->y = s * a->y;
	r->z = s * a->z;
}

float vec_len_sq(const vec_t *a)
{
	return vec_dot(a, a);
}

float vec_len(const vec_t *a)
{
	return sqrtf(vec_len_sq(a));
}

void vec_norm(vec_t *r, const vec_t *a)
{
	vec_scale(r, 1 / vec_len(a), a);
}

void vec_add(vec_t *r, const vec_t *a, const vec_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

void vec_set(vec_t *r, const vec_t *a)
{
	*r = *a;
}

void vec_fma(vec_t *r, const vec_t *a, float s, const vec_t *b)
{
	r->x = a->x + s * b->x;
	r->y = a->y + s * b->y;
	r->z = a->z + s * b->z;
}

void vec_sub(vec_t *r, const vec_t *a, const vec_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
}

void vec_nmul(vec_t *r, const vec_t *a, const vec_t *b)
{
	r->x = a->x * b->x;
	r->y = a->y * b->y;
	r->z = a->z * b->z;
}

void vec_clamp(vec_t *r, const vec_t *a, const vec_t *lo, const vec_t *hi)
{
	r->x = (a->x > lo->x ? a->x < hi->x ? a->x : hi->x : lo->x);
	r->y = (a->y > lo->y ? a->y < hi->y ? a->y : hi->y : lo->y);
	r->z = (a->z > lo->z ? a->z < hi->z ? a->z : hi->z : lo->z);
}

float vec_cos(const vec_t *a, const vec_t *b)
{
	return vec_dot(a, b) / (vec_len(a) * vec_len(b));
}

float vec_proj_s(const vec_t *a, const vec_t *b)
{
	return vec_dot(a, b) / vec_len(b);
}

void vec_proj_v(vec_t *r, const vec_t *a, const vec_t *b)
{
	vec_scale(r, vec_dot(a, b) / vec_len_sq(b), b);
}

void vec_rej_v(vec_t *r, const vec_t *a, const vec_t *b)
{
	vec_t v;

	vec_proj_v(&v, a, b);
	vec_sub(r, a, &v);
}


void vec2_sub(vec2_t *r, const vec2_t *a, const vec2_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
}
