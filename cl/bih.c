#include "bih.h"
#include "box.h"
#include "prim.h"
#include "scene.h"
#include "vec.h"

prim_t *bih_trace(scene_t *scene, vec3_t *p, vec3_t *d, real_t *m, prim_t *u)
{
	int	nodes[2048];
	box_t	boxes[2048];
	int	n		= 1;
	prim_t *t		= NULL;
	real_t	l		= INFINITY;

	nodes[0] = 0;
	boxes[0] = scene->box;

	for (int i = 0; i < n; i++)
	{
		bih_t *	node	= &scene->p_bih[nodes[i]];
		box_t *	box	= &boxes[i];
		int	a	= node->val & 3;
		int	v	= node->val >> 2;

		if (box_trace(box, p, d, l) == INFINITY)
		{
			continue;
		}

		if (a == 3)
		{
			/* Leaf node */
			for (int j = 0; j < node->num; j++)
			{
				prim_t *n_t = &scene->p_prim[v + j];
				real_t	n_l = prim_trace(n_t, p, d, l, u);

				if (n_l < l)
				{
					t = n_t;
					l = n_l;
				}
			}
		}
		else
		{
			real_t	dv	= 0;
			int	l;
			int	r;

			switch (a)
			{
				case 0 : dv = d->x; break;
				case 1 : dv = d->y; break;
				case 2 : dv = d->z; break;
			}

			if (dv > 0)
			{
				l = n++;
				r = n++;
			}
			else
			{
				r = n++;
				l = n++;
			}

			nodes[l] = v + 0;
			boxes[l] = *box;
			boxes[l].max[a] = node->clip[0];

			nodes[r] = v + 1;
			boxes[r] = *box;
			boxes[r].min[a] = node->clip[1];
		}
	}

	*m = l;

	return t;
}
