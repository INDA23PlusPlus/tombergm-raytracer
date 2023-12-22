#include <tgmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "sph.h"
#include "tri.h"
#include "vec.h"

#define DEF_PUSH(t, n) \
static t *n ## _push(const t *v) \
{ \
	if (n ## _num + 1 > n ## _siz) \
	{ \
		size_t siz; \
 \
		if (n ## _siz == 0) \
		{ \
			n ## _siz = 1; \
		} \
		else \
		{ \
			n ## _siz = n ## _siz * 2; \
		} \
 \
		siz = sizeof(t) * n ## _siz; \
 \
		if (n ## _buf == NULL) \
		{ \
			n ## _buf = malloc(siz); \
		} \
		else \
		{ \
			n ## _buf = realloc(n ## _buf, siz); \
		} \
	} \
 \
	if (n ## _buf == NULL) \
	{ \
		return NULL; \
	} \
 \
	if (v != NULL) \
	{ \
		n ## _buf[n ## _num] = *v; \
	} \
 \
	return &n ## _buf[n ## _num++]; \
}

#define PRIM_TRI	0
#define PRIM_SPH	1

typedef struct
{
	int	type;
	void *	ptr;
} prim_t;

static prim_t *	prim_buf;
static int	prim_num;
static int	prim_siz;

DEF_PUSH(prim_t, prim)

void prim_list_build(const scene_t *scene)
{
	for (int i = 0; i < scene->n_tri; i++)
	{
		prim_t prim;

		prim.type = PRIM_TRI;
		prim.ptr = &scene->p_tri[i];

		prim_push(&prim);
	}

	for (int i = 0; i < scene->n_sph; i++)
	{
		prim_t prim;

		prim.type = PRIM_SPH;
		prim.ptr = &scene->p_sph[i];

		prim_push(&prim);
	}
}



#define min2(a, b)	fmin(a, b)
#define min3(a, b, c)	min2(min2(a, b), c)
#define max2(a, b)	fmax(a, b)
#define max3(a, b, c)	max2(a, max2(b, c))

typedef struct
{
	real_t	min[3];
	real_t	max[3];
} bbox_t;

static void bbox_join(bbox_t *a, const bbox_t *b)
{
	for (int i = 0; i < 3; i++)
	{
		a->min[i] = min2(a->min[i], b->min[i]);
		a->max[i] = max2(a->max[i], b->max[i]);
	}
}

static void tri_get_bbox(const tri_t *tri, bbox_t *box)
{
	box->min[0] = min3(tri->a.x, tri->b.x, tri->c.x);
	box->max[0] = max3(tri->a.x, tri->b.x, tri->c.x);
	box->min[1] = min3(tri->a.y, tri->b.y, tri->c.y);
	box->max[1] = max3(tri->a.y, tri->b.y, tri->c.y);
	box->min[2] = min3(tri->a.z, tri->b.z, tri->c.z);
	box->max[2] = max3(tri->a.z, tri->b.z, tri->c.z);
}

static void sph_get_bbox(const sph_t *sph, bbox_t *box)
{
	box->min[0] = sph->c.x - sph->r;
	box->max[0] = sph->c.x + sph->r;
	box->min[1] = sph->c.y - sph->r;
	box->max[1] = sph->c.y + sph->r;
	box->min[2] = sph->c.z - sph->r;
	box->max[2] = sph->c.z + sph->r;
}

static void prim_get_bbox(const prim_t *prim, bbox_t *box)
{
	switch (prim->type)
	{
		case PRIM_TRI	: return tri_get_bbox(prim->ptr, box);
		case PRIM_SPH	: return sph_get_bbox(prim->ptr, box);
		default		: abort();
	}
}

static real_t tri_trace2(	const tri_t *tri, vec3_t *p, vec3_t *d,
				real_t m, void *prev)
{
	vec3_t	q;
	real_t	l;

	if (tri == prev)
	{
		return INFINITY;
	}

	l = vec3_dot(&tri->a, &tri->n) - vec3_dot(p, &tri->n);
	l = l / vec3_dot(d, &tri->n);

	if (0 >= l || l >= m)
	{
		return INFINITY;
	}

	vec3_fma(&q, p, l, d);

	{
		real_t	x	= vec3_dot(&q, &tri->i);
		real_t	y	= vec3_dot(&q, &tri->j);
		real_t	l_1	=
		(
			+ (tri->pb.y - tri->pc.y) * (x - tri->pc.x)
			- (tri->pb.x - tri->pc.x) * (y - tri->pc.y)
		) * tri->d;
		real_t	l_2	=
		(
			- (tri->pa.y - tri->pc.y) * (x - tri->pc.x)
			+ (tri->pa.x - tri->pc.x) * (y - tri->pc.y)
		) * tri->d;
		real_t	l_3	= 1 - l_1 - l_2;

		if (l_1 < 0 || l_2 < 0 || l_3 < 0)
		{
			/* Intersection lies outside triangle */
			return INFINITY;
		}
	}

	return l;
}

static real_t sph_trace2(	const sph_t *sph, vec3_t *p, vec3_t *d,
				real_t m, void *prev)
{
	vec3_t o;
	vec3_t v;
	vec3_t w;
	real_t h;
	real_t l;
	real_t a;

	vec3_sub(&o, &sph->c, p);
	l = vec3_dot(&o, d);

	if (l <= 0)
	{
		/* Ray is moving away */
		return INFINITY;
	}

	vec3_scale(&v, l, d);
	vec3_sub(&w, &v, &o);
	h = vec3_len(&w);

	if (h > sph->r)
	{
		/* Ray intersection is outside sphere radius */
		return INFINITY;
	}

	a = 1 - (sph->r - h) / sph->r;
	a = sqrt(1 - a * a);

	if (sph == prev)
	{
		/* Hit the back surface with transmission rays */
		l = l + sph->r * a;
	}
	else
	{
		/* Otherwise hit the front surface */
		l = l - sph->r * a;
	}

	if (0 >= l || l >= m)
	{
		return INFINITY;
	}

	return l;
}

static real_t prim_trace(const prim_t *prim, vec3_t *p, vec3_t *d, real_t m,
				prim_t *u)
{
	void *	prev;

	if (u != NULL)
	{
		prev = u->ptr;
	}
	else
	{
		prev = NULL;
	}

	switch (prim->type)
	{
		case PRIM_TRI	: return tri_trace2(prim->ptr, p, d, m, prev);
		case PRIM_SPH	: return sph_trace2(prim->ptr, p, d, m, prev);
		default		: abort();
	}
}

static int bbox_trace(const bbox_t *box, vec3_t *p, vec3_t *d, real_t m)
{
	real_t l_ax = (box->min[0] - p->x) / d->x;
	real_t l_bx = (box->max[0] - p->x) / d->x;
	real_t l_ay = (box->min[1] - p->y) / d->y;
	real_t l_by = (box->max[1] - p->y) / d->y;
	real_t l_az = (box->min[2] - p->z) / d->z;
	real_t l_bz = (box->max[2] - p->z) / d->z;

	real_t l_f = max3(min2(l_ax, l_bx), min2(l_ay, l_by), min2(l_az, l_bz));
	real_t l_b = min3(max2(l_ax, l_bx), max2(l_ay, l_by), max2(l_az, l_bz));

	if (0 > l_b || l_f > min2(l_b, m))
	{
		return 0;
	}

	return 1;
}


static void tri_map(const tri_t *tri, vec3_t *q, vec2_t *uv)
{
	vec3_t	p;
	vec3_t	i;
	vec3_t	j;
	vec2_t	st;

	vec3_sub(&p, q, &tri->a);

	vec3_cross(&i, &p, &tri->iw);
	vec3_cross(&j, &p, &tri->jw);

	st.x = sqrt(vec3_len_sq(&j) * tri->td);
	st.y = sqrt(vec3_len_sq(&i) * tri->td);

	vec2_fma(uv, &tri->at, st.x, &tri->iv);
	vec2_fma(uv, uv, st.y, &tri->jv);
}

static void tri_hit(const tri_t *tri, ray_t *ray)
{
	vec3_set(&ray->n, &tri->n);

	ray->mat = tri->mat;

	if (mat_has_tex(ray->mat))
	{
		tri_map(tri, &ray->q, &ray->uv);

		ray->tu = tri->tu;
		ray->tv = tri->tv;
	}
}

static void sph_hit(const sph_t *sph, ray_t *ray)
{
	vec3_sub(&ray->n, &ray->q, &sph->c);
	vec3_scale(&ray->n, 1 / sph->r, &ray->n);

	ray->mat = sph->mat;

	if (mat_has_tex(ray->mat))
	{
		ray->uv.x = 0.5 + asin(ray->n.x) / M_PI;
		ray->uv.y = 0.5 + asin(ray->n.z) / M_PI;
	}

}

void prim_hit(const prim_t *prim, ray_t *ray)
{
	switch (prim->type)
	{
		case PRIM_TRI	: return tri_hit(prim->ptr, ray);
		case PRIM_SPH	: return sph_hit(prim->ptr, ray);
		default		: abort();
	}
}


typedef struct
{
	int		val;
	union
	{
		real_t	clip[2];
		int	num;
	};
} bih_t;

static bih_t *	bih_buf;
static int	bih_num;
static int	bih_siz;

DEF_PUSH(bih_t, bih)

static void bih_split(bih_t *bih, bbox_t *box, int l, int r, int a)
{
	bbox_t	l_box	= *box;
	bbox_t	r_box	= *box;
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
		prim_t *prim		= &prim_buf[i];
		bbox_t	prim_box;
		real_t	lo;
		real_t	ro;

		prim_get_bbox(prim, &prim_box);

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

				memcpy(	&prim_buf[m + 1],
					&prim_buf[m],
					sizeof(prim_t) * (i - m)
					);

				prim_buf[m++] = t;
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
		int	v	= bih_num;

		l_box.max[a]	= l_max;
		r_box.min[a]	= r_min;

		bih->val	= ((v << 2) | a);
		bih->clip[0]	= l_max;
		bih->clip[1]	= r_min;

		bih_push(NULL);
		bih_push(NULL);

		bih_split(&bih_buf[v + 0], &l_box, l, m, -1);
		bih_split(&bih_buf[v + 1], &r_box, m, r, -1);
	}
	else if (l != m)
	{
		if (l_box.max[a] > l_max)
		{
			/* Plane is outside volume, split with empty space */
			int	v	= bih_num;

			l_box.max[a]	= l_max;
			r_box.min[a]	= r_min;

			bih->val	= ((v << 2) | a);
			bih->clip[0]	= l_max;
			bih->clip[1]	= r_min;

			bih_push(NULL);
			bih_push(NULL);

			bih_split(&bih_buf[v + 0], &l_box, l, m, -1);
			bih_buf[v + 1].val = 3;
			bih_buf[v + 1].num = 0;
		}
		else
		{
			/* Recurse along same axis, splitting farther left */
			bih_split(bih, &l_box, l, m, a);
		}
	}
	else if (r != m)
	{
		if (r_box.min[a] < r_min)
		{
			/* Plane is outside volume, split with empty space */
			int	v	= bih_num;

			l_box.max[a]	= l_max;
			r_box.min[a]	= r_min;

			bih->val	= ((v << 2) | a);
			bih->clip[0]	= l_max;
			bih->clip[1]	= r_min;

			bih_push(NULL);
			bih_push(NULL);

			bih_buf[v + 0].val = 3;
			bih_buf[v + 0].num = 0;
			bih_split(&bih_buf[v + 1], &r_box, m, r, -1);
		}
		else
		{
			/* Recurse along same axis, splitting farther right */
			bih_split(bih, &r_box, m, r, a);
		}
	}
}

static int check_depth(const bih_t *bih)
{
	if ((bih->val & 3) == 3)
	{
		return 1;
	}
	else
	{
		return 1 + max2(check_depth(&bih_buf[(bih->val >> 2) + 0]),
				check_depth(&bih_buf[(bih->val >> 2) + 1]));
	}
}

static int check_size(const bih_t *bih)
{
	if ((bih->val & 3) == 3)
	{
		return bih->num;
	}
	else
	{
		return max2(	check_size(&bih_buf[(bih->val >> 2) + 0]),
				check_size(&bih_buf[(bih->val >> 2) + 1]));
	}
}

static bbox_t scene_box;

void bih_build(scene_t *scene)
{
	prim_list_build(scene);

	if (prim_num == 0)
	{
		return;
	}

	fprintf(stderr, "Primitives: %i\n", prim_num);

	for (int i = 0; i < prim_num; i++)
	{
		bbox_t prim_box;

		prim_get_bbox(&prim_buf[i], &prim_box);

		if (i == 0)
		{
			scene_box = prim_box;
		}
		else
		{
			bbox_join(&scene_box, &prim_box);
		}
	}

	bih_split(bih_push(NULL), &scene_box, 0, prim_num, -1);

	fprintf(stderr, "Nodes: %i\n", bih_num);
	fprintf(stderr, "Max depth: %i\n", check_depth(&bih_buf[0]));
	fprintf(stderr, "Max leaf size: %i\n", check_size(&bih_buf[0]));
}

void *bih_trace(vec3_t *p, vec3_t *d, real_t *m, prim_t *u)
{
	bih_t *	nodes[2048];
	bbox_t	boxes[2048];
	int	n		= 1;
	prim_t *t		= NULL;
	real_t	l		= INFINITY;

	nodes[0] = &bih_buf[0];
	boxes[0] = scene_box;

	for (int i = 0; i < n; i++)
	{
		bih_t *	node	= nodes[i];
		bbox_t *box	= &boxes[i];
		int	a	= node->val & 3;
		int	v	= node->val >> 2;

		if (!bbox_trace(box, p, d, l))
		{
			continue;
		}

		if (a == 3)
		{
			/* Leaf node */
			for (int j = 0; j < node->num; j++)
			{
				prim_t *n_t = &prim_buf[v + j];
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
				nodes[l] = &bih_buf[v + 0];
				boxes[l] = *box;
				boxes[l].max[a] = node->clip[0];

				r = n++;
				nodes[r] = &bih_buf[v + 1];
				boxes[r] = *box;
				boxes[r].min[a] = node->clip[1];
			}
			else
			{
				r = n++;
				nodes[r] = &bih_buf[v + 1];
				boxes[r] = *box;
				boxes[r].min[a] = node->clip[1];

				l = n++;
				nodes[l] = &bih_buf[v + 0];
				boxes[l] = *box;
				boxes[l].max[a] = node->clip[0];
			}
		}
	}

	*m = l;

	return t;
}
