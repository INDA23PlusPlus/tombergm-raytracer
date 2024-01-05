#include "bih.h"
#include "box.h"
#include "prim.h"
#include "scene.h"
#include "vec.h"

int bih_trace(SCENE, vec3_t *p, vec3_t *d, real_t *m, int u)
{
	int	i	= 0;
	int	t	= -1;
	real_t	l	= INFINITY;

	while (i != scene_n_bih)
	{
		bih_t *	bih = BIH(i);

		if (box_trace(&bih->box, p, d, l) == INFINITY)
		{
			i = bih->next;
		}
		else
		{
			for (int j = 0; j < bih->prim_num; j++)
			{
				int	k = bih->prim_idx + j;
				real_t	m;

				m = prim_trace(scene, PRIM(k), p, d, l, u == k);

				if (l > m)
				{
					t = k;
					l = m;
				}
			}

			i++;
		}
	}

	*m = l;

	return t;
}
