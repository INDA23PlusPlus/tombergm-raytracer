#include "bih.h"
#include "box.h"
#include "prim.h"
#include "scene.h"
#include "vec.h"

#if 0
int bih_trace(SCENE, vec3_t *p, vec3_t *d, real_t *m, int u)
{
#define N 512
	int	nodes[N];
	int	n		= 1;
	int	t		= -1;
	real_t	l		= INFINITY;

	nodes[0] = 0;

	for (int i = 0; i != n; i = ((i + 1) & (N - 1)))
	{
		bih_t *	node	= BIH(nodes[i]);
		int	a	= node->val & 3;
		int	v	= node->val >> 2;

		if (box_trace(&node->box, p, d, l) == INFINITY)
		{
			continue;
		}

		if (a == 3)
		{
			/* Leaf node */
			for (int j = 0; j < node->num; j++)
			{
				prim_t *n_t = PRIM(v + j);
				real_t	n_l = prim_trace(	scene, n_t,
								p, d, l,
								u == v + j);

				if (n_l < l)
				{
					t = v + j;
					l = n_l;
				}
			}
		}
		else
		{
			int	l;
			int	r;
			real_t	dv	= 0;

			switch (a)
			{
				case 0 : dv = d->x; break;
				case 1 : dv = d->y; break;
				case 2 : dv = d->z; break;
			}

			if (dv > 0)
			{
				l = (n++ & (N - 1));
				r = (n++ & (N - 1));
			}
			else
			{
				r = (n++ & (N - 1));
				l = (n++ & (N - 1));
			}

			nodes[l] = v + 0;
			nodes[r] = v + 1;
		}
	}

	*m = l;

	return t;
}
#else
int bih_trace(SCENE, vec3_t *p, vec3_t *d, real_t *m, int u)
{
	int	t	= -1;
	real_t	l	= INFINITY;

	for (int i = 0; i < scene_n_prim; i++)
	{
		prim_t *n_t = PRIM(i);
		real_t	n_l = prim_trace(scene, n_t, p, d, l, u == i);

		if (n_l < l)
		{
			t = i;
			l = n_l;
		}
	}

	*m = l;

	return t;
}
#endif
