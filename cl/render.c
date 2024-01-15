#include "cam.h"
#include "ray.h"
#include "scene.h"
#include "tex.h"
#include "vec.h"

__kernel
void render(	__global unsigned char *pb, __global vec3_t *sb,
		cam_t *cam, SCENE, int sn, unsigned rand)
{
	int	w	= get_global_size(0);
	int	h	= get_global_size(1);
	int	x	= get_global_id(0);
	int	y	= get_global_id(1);
	vec3_t	p	= cam->p;
	vec3_t	uv	= cam->uv;
	vec3_t	fv	= cam->fv;
	vec3_t	rv	= cross(uv, fv);
	real_t	l	= cam->l;
	real_t	r	= cam->r;
	real_t	t	= cam->t;
	real_t	b	= cam->b;
	real_t	n	= cam->n;
	real_t	ap	= cam->ap;

	rand =	(rand ^ x) * (y + 1);
	rand =	((rand << (((y + 1) * (x + 1)) & 31))		|
		(rand >> (32 - (((y + 1) * (x + 1)) & 31))))	;
	rand = rand + *(int *) &sb[y * w + x] * sn;

	{
		__global
		unsigned char *	pix	= &pb[(y * w + x) * 4];
		vec3_t		c	= 0;
		vec3_t		q;
		vec3_t		d;
		real_t		rs	= l + x * (r - l) / (w - 1);
		real_t		us	= b + y * (t - b) / (h - 1);
		real_t		fs	= n;

		rs = rs + (r - l) / (w - 1) * (0.5 - flt_rand(&rand));
		us = us + (t - b) / (h - 1) * (0.5 - flt_rand(&rand));

		q = p + rs * rv + us * uv + fs * fv;

		{
			real_t l = ap * sqrt(flt_rand(&rand));
			real_t r = flt_rand(&rand) * 2 * M_PI;
			p = p + l * (cos(r) * rv + sin(r) * uv);
		}

		d = normalize(q - p);

		ray_trace(scene, &c, &p, &d, &rand);

		if (sn != 0)
		{
			__global
			vec3_t *s = &sb[y * w + x];

			if (sn == 1)
			{
				*s = c;
			}
			else
			{
				*s = *s + c;
				c = *s / sn;
			}
		}

		c = clamp(pow(c, 1.f / 2.2f), 0, 1);

		pix[0] = 0.5 + c.z * 255;
		pix[1] = 0.5 + c.y * 255;
		pix[2] = 0.5 + c.x * 255;
	}
}
