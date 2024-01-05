#include "scene.h"
#include "tex.h"
#include "vec.h"

void tex_sample(SCENE, const tex_t *tex, const vec2_t *uv,
		vec3_t *c, vec3_t *n, real_t *r)
{
	const sampler_t s =	CLK_NORMALIZED_COORDS_TRUE	|
				CLK_ADDRESS_REPEAT		|
				CLK_FILTER_LINEAR		;

	if (tex->c != -1)
	{
		float4 coord = {uv->x, uv->y, (float) tex->c, 0.f};
		float4 tc;

		tc = read_imagef(IMG, s, coord);

		*c = tc.xyz;
	}

	if (tex->n != -1)
	{
		float4 coord = {uv->x, uv->y, (float) tex->n, 0.f};
		float4 tn;

		tn = read_imagef(IMG, s, coord);
		tn = tn * 2 - 1;
		tn = normalize(tn);

		*n = tn.xyz;
	}

	if (tex->r != -1)
	{
		float4 coord = {uv->x, uv->y, (float) tex->r, 0.f};
		float4 tr;

		tr = read_imagef(IMG, s, coord);

		*r = tr.x;
	}
}
