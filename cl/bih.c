#include "bih.h"
#include "box.h"
#include "prim.h"
#include "ray.h"
#include "scene.h"
#include "vec.h"

void bih_trace(SCENE, ray_t *ray)
{
	int i = 0;

	while (i != scene_n_bih)
	{
		bih_t *	bih = BIH(i);

		if (box_trace(&bih->box, ray) == INFINITY)
		{
			i = bih->next;
		}
		else
		{
			for (int j = 0; j < bih->prim_num; j++)
			{
				int	t = bih->prim_idx + j;
				real_t	l;

				l = prim_trace(	scene, PRIM(t), ray,
						t == ray->prev);

				if (ray->l > l)
				{
					ray->curr = t;
					ray->l = l;
				}
			}

			i++;
		}
	}
}
