#include <tgmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bih.h"
#include "box.h"
#include "prim.h"
#include "scene.h"
#include "vec.h"
#include "vector.h"

static void bih_split(	scene_t *scene, vector_t *bih_vec,
			int bih_n, box_t *box,
			int l, int r, int a)
{
	bih_t *	bih	= vector_at(bih_vec, bih_n);
	box_t	l_box	= *box;
	box_t	r_box	= *box;
	int	m	= l;
	real_t	w	= -1;
	real_t	l_max;
	real_t	r_min;

	if (r - l < 4)
	{
		/* No need to split further, make leaf */
		bih->val = ((l << 2) | 3);
		bih->num = r - l;

		return;
	}

	if (a == -1)
	{
		/* Pick an axis */
		for (int i = 0; i < 3; i++)
		{
			real_t iw = box->max[i] - box->min[i];

			if (w < iw)
			{
				a = i;
				w = iw;
			}
		}
	}
	else
	{
		/* Use provided axis */
		w = box->max[a] - box->min[a];
	}

	/* Set split planes */
	l_max = l_box.max[a] - w / 2;
	r_min = r_box.min[a] + w / 2;

	if (l_box.max[a] == l_max || r_box.min[a] == r_min)
	{
		/* Splitting leads nowhere, make leaf */
		bih->val = ((l << 2) | 3);
		bih->num = r - l;

		return;
	}

	l_box.max[a] = l_max;
	r_box.min[a] = r_min;

	/* Keep track of volume of contained primitives */
	l_max = l_box.min[a];
	r_min = r_box.max[a];

	for (int i = l; i < r; i++)
	{
		prim_t *prim		= &scene->p_prim[i];
		box_t	prim_box;
		real_t	lo;
		real_t	ro;

		prim_get_box(prim, &prim_box);

		/* Compute amount of overlap for each side */
		lo = max2(l_box.max[a] - prim_box.min[a], 0);
		ro = max2(prim_box.max[a] - r_box.min[a], 0);

		/* Pick the side with most overlap */
		if (lo > ro)
		{
			/* Sort left */
			if (m != i)
			{
				prim_t	t = *prim;

				memcpy(	&scene->p_prim[m + 1],
					&scene->p_prim[m],
					sizeof(prim_t) * (i - m)
					);

				scene->p_prim[m++] = t;
			}
			else
			{
				m++;
			}

			l_max = max2(l_max, prim_box.max[a]);
		}
		else
		{
			/* Sort right */
			r_min = min2(r_min, prim_box.min[a]);
		}
	}

	if (l != m && r != m)
	{
		/* Regular split */
		int	v	= bih_vec->num;

		l_box.max[a]	= l_max;
		r_box.min[a]	= r_min;

		bih->val	= ((v << 2) | a);
		bih->clip[0]	= l_max;
		bih->clip[1]	= r_min;

		vector_push(bih_vec, NULL);
		vector_push(bih_vec, NULL);

		bih_split(scene, bih_vec, v + 0, &l_box, l, m, -1);
		bih_split(scene, bih_vec, v + 1, &r_box, m, r, -1);
	}
	else if (l != m)
	{
		if (l_box.max[a] > l_max)
		{
			/* Plane is outside volume, split with empty space */
			int	v	= bih_vec->num;

			l_box.max[a]	= l_max;
			r_box.min[a]	= r_min;

			bih->val	= ((v << 2) | a);
			bih->clip[0]	= l_max;
			bih->clip[1]	= r_min;

			vector_push(bih_vec, NULL);
			vector_push(bih_vec, NULL);

			bih_split(scene, bih_vec, v + 0, &l_box, l, m, -1);
			bih_split(scene, bih_vec, v + 1, &l_box, m, r, -1);
		}
		else
		{
			/* Recurse along same axis, splitting farther left */
			bih_split(scene, bih_vec, bih_n, &l_box, l, m, a);
		}
	}
	else if (r != m)
	{
		if (r_box.min[a] < r_min)
		{
			/* Plane is outside volume, split with empty space */
			int	v	= bih_vec->num;

			l_box.max[a]	= l_max;
			r_box.min[a]	= r_min;

			bih->val	= ((v << 2) | a);
			bih->clip[0]	= l_max;
			bih->clip[1]	= r_min;

			vector_push(bih_vec, NULL);
			vector_push(bih_vec, NULL);

			bih_split(scene, bih_vec, v + 0, &r_box, l, m, -1);
			bih_split(scene, bih_vec, v + 1, &r_box, m, r, -1);
		}
		else
		{
			/* Recurse along same axis, splitting farther right */
			bih_split(scene, bih_vec, bih_n, &r_box, m, r, a);
		}
	}
}

static int check_depth(const vector_t *bih_vec, int bih_n)
{
	const bih_t *bih = vector_at(bih_vec, bih_n);

	if ((bih->val & 3) == 3)
	{
		return 1;
	}
	else
	{
		return 1 + max2(check_depth(bih_vec, (bih->val >> 2) + 0),
				check_depth(bih_vec, (bih->val >> 2) + 1));
	}
}

static int check_size(const vector_t *bih_vec, int bih_n)
{
	const bih_t *bih = vector_at(bih_vec, bih_n);

	if ((bih->val & 3) == 3)
	{
		return bih->num;
	}
	else
	{
		return max2(	check_size(bih_vec, (bih->val >> 2) + 0),
				check_size(bih_vec, (bih->val >> 2) + 1));
	}
}

void bih_build(scene_t *scene)
{
	vector_t bih_vec;

	for (int i = 0; i < scene->n_prim; i++)
	{
		box_t prim_box;

		prim_get_box(&scene->p_prim[i], &prim_box);

		if (i == 0)
		{
			scene->box = prim_box;
		}
		else
		{
			box_join(&scene->box, &prim_box);
		}
	}

	vector_init(&bih_vec, sizeof(bih_t));
	vector_push(&bih_vec, NULL);

	bih_split(scene, &bih_vec, 0, &scene->box, 0, scene->n_prim, -1);

	scene->n_bih = bih_vec.num;
	scene->p_bih = bih_vec.buf;

	fprintf(stderr, "Nodes: %i\n", bih_vec.num);
	fprintf(stderr, "Max depth: %i\n", check_depth(&bih_vec, 0));
	fprintf(stderr, "Max leaf size: %i\n", check_size(&bih_vec, 0));
}

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
