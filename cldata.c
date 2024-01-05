#include <CL/cl.h>
#include <string.h>
#include <CL/opencl.h>
#include "cldata.h"
#include "bih.h"
#include "box.h"
#include "cl/box.h"
#include "mat.h"
#include "prim.h"
#include "scene.h"
#include "sph.h"
#include "tex.h"
#include "tri.h"

static int		n_img;
static const tex_t *	p_tex[512];
static void *		p_buf[512];

static void *clmalloc(cl_context c, cl_command_queue q, size_t size, cl_mem *m)
{
	void *ptr = malloc(size);

	*m = clCreateBuffer(	c, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
				size, ptr, NULL);

	clEnqueueMapBuffer(	q, *m, CL_TRUE,
				CL_MAP_WRITE_INVALIDATE_REGION,
				0, size,
				0, NULL, NULL, NULL);

	return ptr;
}

static void clunmap(cl_context c, cl_command_queue q, void *ptr, cl_mem *m)
{
	clEnqueueUnmapMemObject(q, *m, ptr, 0, NULL, NULL);
}

static void copy_vec2(vec2_cl_t *vec_cl, const vec2_t *vec)
{
	vec_cl->x = vec->x;
	vec_cl->y = vec->y;
}

static void copy_vec3(vec3_cl_t *vec_cl, const vec3_t *vec)
{
	vec_cl->x = vec->x;
	vec_cl->y = vec->y;
	vec_cl->z = vec->z;
}

static void copy_box(	cl_context c, cl_command_queue q,
			box_cl_t *box_cl, const box_t *box,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	box_cl->min[0] = box->min[0];
	box_cl->min[1] = box->min[1];
	box_cl->min[2] = box->min[2];
	box_cl->max[0] = box->max[0];
	box_cl->max[1] = box->max[1];
	box_cl->max[2] = box->max[2];
}

static void copy_tex(	cl_context c, cl_command_queue q,
			tex_cl_t *tex_cl, const tex_t *tex,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	tex_cl->w = tex->w;
	tex_cl->h = tex->h;

	if (tex->c != NULL)
	{
		p_tex[n_img] = tex;
		p_buf[n_img] = tex->c;
		tex_cl->c = n_img++;
	}
	else
	{
		tex_cl->c = -1;
	}

	if (tex->n != NULL)
	{
		p_tex[n_img] = tex;
		p_buf[n_img] = tex->n;
		tex_cl->n = n_img++;
	}
	else
	{
		tex_cl->n = -1;
	}

	if (tex->r != NULL)
	{
		p_tex[n_img] = tex;
		p_buf[n_img] = tex->r;
		tex_cl->r = n_img++;
	}
	else
	{
		tex_cl->r = -1;
	}
}

static void copy_mat(	cl_context c, cl_command_queue q,
			mat_cl_t *mat_cl, const mat_t *mat,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	if (mat->tex != NULL)
	{
		mat_cl->tex = mat->tex - scene->p_tex;
	}
	else
	{
		mat_cl->tex = -1;
	}

	copy_vec3(&mat_cl->col, &mat->col);

	mat_cl->dif = mat->dif;
	mat_cl->amb = mat->amb;
	mat_cl->ref = mat->ref;
	mat_cl->tra = mat->tra;
	mat_cl->ind = mat->ind;
	mat_cl->flg = mat->flg;
}

static void copy_tri(	cl_context c, cl_command_queue q,
			tri_cl_t *tri_cl, const tri_t *tri,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	copy_vec3(&tri_cl->a, &tri->a);
	copy_vec3(&tri_cl->b, &tri->b);
	copy_vec3(&tri_cl->c, &tri->c);

	if (tri->mat != NULL)
	{
		tri_cl->mat = tri->mat - scene->p_mat;
	}
	else
	{
		tri_cl->mat = -1;
	}

	copy_vec2(&tri_cl->at, &tri->at);
	copy_vec2(&tri_cl->bt, &tri->bt);
	copy_vec2(&tri_cl->ct, &tri->ct);

	copy_vec3(&tri_cl->n, &tri->n);
	copy_vec3(&tri_cl->i, &tri->i);
	copy_vec3(&tri_cl->j, &tri->j);
	copy_vec2(&tri_cl->pa, &tri->pa);
	copy_vec2(&tri_cl->pb, &tri->pb);
	copy_vec2(&tri_cl->pc, &tri->pc);

	tri_cl->d = tri->d;

	copy_vec2(&tri_cl->iv, &tri->iv);
	copy_vec2(&tri_cl->jv, &tri->jv);
	copy_vec3(&tri_cl->iw, &tri->iw);
	copy_vec3(&tri_cl->jw, &tri->jw);

	tri_cl->td = tri->td;

	copy_vec3(&tri_cl->tu, &tri->tu);
	copy_vec3(&tri_cl->tv, &tri->tv);
}

static void copy_sph(	cl_context c, cl_command_queue q,
			sph_cl_t *sph_cl, const sph_t *sph,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	copy_vec3(&sph_cl->c, &sph->c);

	sph_cl->r = sph->r;

	sph_cl->mat = sph->mat - scene->p_mat;
}

static void copy_prim(	cl_context c, cl_command_queue q,
			prim_cl_t *prim_cl, const prim_t *prim,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	prim_cl->type = prim->type;

	switch (prim->type)
	{
		case PRIM_TRI:
		{
			prim_cl->idx = (tri_t *) prim->ptr - scene->p_tri;
			break;
		}
		case PRIM_SPH:
		{
			prim_cl->idx = (sph_t *) prim->ptr - scene->p_sph;
			break;
		}
	}
}

static void build_bih(	bih_cl_t *bih_cl, bih_t *bih,
			box_cl_t *box_cl,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	int	v	= bih->val >> 2;
	int	a	= bih->val & 3;

	bih_cl->box = *box_cl;

	if (a != 3)
	{
		bih_t *		l_bih		= &scene->p_bih[v + 0];
		int		l_num_cl	= scene_cl->n_bih++;
		bih_cl_t *	l_bih_cl	= &scene_cl->p_bih[l_num_cl];
		box_cl_t	l_box_cl	= *box_cl;

		l_box_cl.max[a] = bih->clip[0];
		build_bih(l_bih_cl, l_bih, &l_box_cl, scene_cl, scene);

		bih_t *		r_bih		= &scene->p_bih[v + 1];
		int		r_num_cl	= scene_cl->n_bih++;
		bih_cl_t *	r_bih_cl	= &scene_cl->p_bih[r_num_cl];
		box_cl_t	r_box_cl	= *box_cl;

		r_box_cl.min[a] = bih->clip[1];
		build_bih(r_bih_cl, r_bih, &r_box_cl, scene_cl, scene);

		bih_cl->prim_num = 0;
		bih_cl->prim_idx = 0;
	}
	else
	{
		bih_cl->prim_idx = v;
		bih_cl->prim_num = bih->num;
	}

	bih_cl->next = scene_cl->n_bih;
}

static void copy_images(cl_context c, cl_command_queue q, scene_cl_t *scene_cl)
{
	int		w	= 4096;
	int		h	= 4096;
	int		p	= 4;
	int		r	= w * p;
	int		s	= r * h;
	cl_image_format	fmt;
	cl_image_desc	dsc;

	scene_cl->p_img = malloc(s * n_img);

	for (int i = 0; i < n_img; i++)
	{
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				vec2_t		uv;
				vec3_t		c;
				unsigned char *	b;

				uv.x = (real_t) x / (w - 1);
				uv.y = (real_t) y / (h - 1);

				c = tex_sample_buf(p_tex[i], &uv, p_buf[i]);

				b = scene_cl->p_img;
				b = &b[i * s + y * r + x * p];

				b[0] = 0.5 + c.x * 255;
				b[1] = 0.5 + c.y * 255;
				b[2] = 0.5 + c.z * 255;
				b[3] = 255;
			}
		}
	}

	fmt.image_channel_order		= CL_RGBA;
	fmt.image_channel_data_type	= CL_UNORM_INT8;

	dsc.image_type		= CL_MEM_OBJECT_IMAGE2D_ARRAY;
	dsc.image_width		= w;
	dsc.image_height	= h;
	dsc.image_depth		= 0;
	dsc.image_array_size	= n_img;
	dsc.image_row_pitch	= r;
	dsc.image_slice_pitch	= s;
	dsc.num_mip_levels	= 0;
	dsc.num_samples		= 0;
	dsc.mem_object		= NULL;

	scene_cl->m_img = clCreateImage(	c,
						CL_MEM_READ_ONLY	|
						CL_MEM_USE_HOST_PTR,
						&fmt, &dsc,
						scene_cl->p_img, NULL);
}

static void copy_scene(	cl_context c, cl_command_queue q,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	{
		scene_cl->n_tex = scene->n_tex;
		scene_cl->p_tex =
			clmalloc(	c, q, sizeof(tex_cl_t) * scene->n_tex,
					&scene_cl->m_tex);

		for (int i = 0; i < scene->n_tex; i++)
		{
			copy_tex(	c, q,
					&scene_cl->p_tex[i],
					&scene->p_tex[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_tex, &scene_cl->m_tex);
	}

	{
		scene_cl->n_mat = scene->n_mat;
		scene_cl->p_mat =
			clmalloc(	c, q, sizeof(mat_cl_t) * scene->n_mat,
					&scene_cl->m_mat);

		for (int i = 0; i < scene->n_mat; i++)
		{
			copy_mat(	c, q,
					&scene_cl->p_mat[i],
					&scene->p_mat[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_mat, &scene_cl->m_mat);
	}

	{
		scene_cl->n_tri = scene->n_tri;
		scene_cl->p_tri =
			clmalloc(	c, q, sizeof(tri_cl_t) * scene->n_tri,
					&scene_cl->m_tri);

		for (int i = 0; i < scene->n_tri; i++)
		{
			copy_tri(	c, q,
					&scene_cl->p_tri[i],
					&scene->p_tri[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_tri, &scene_cl->m_tri);
	}

	{
		scene_cl->n_sph = scene->n_sph;
		scene_cl->p_sph =
			clmalloc(	c, q, sizeof(sph_cl_t) * scene->n_sph,
					&scene_cl->m_sph);

		for (int i = 0; i < scene->n_sph; i++)
		{
			copy_sph(	c, q,
					&scene_cl->p_sph[i],
					&scene->p_sph[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_sph, &scene_cl->m_sph);
	}

	{
		scene_cl->n_prim = scene->n_prim;
		scene_cl->p_prim =
			clmalloc(	c, q, sizeof(prim_cl_t) * scene->n_prim,
					&scene_cl->m_prim);

		for (int i = 0; i < scene->n_prim; i++)
		{
			copy_prim(	c, q,
					&scene_cl->p_prim[i],
					&scene->p_prim[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_prim, &scene_cl->m_prim);
	}

	{
		scene_cl->p_box = clmalloc(	c, q, sizeof(box_cl_t),
						&scene_cl->m_box);

		copy_box(c, q, scene_cl->p_box, &scene->box, scene_cl, scene);

		clunmap(c, q, scene_cl->p_box, &scene_cl->m_box);
	}

	{
		scene_cl->n_bih = 1;
		scene_cl->p_bih =
			clmalloc(	c, q, sizeof(bih_cl_t) * scene->n_bih,
					&scene_cl->m_bih);

		build_bih(	scene_cl->p_bih, scene->p_bih, scene_cl->p_box,
				scene_cl, scene);

		clunmap(c, q, scene_cl->p_bih, &scene_cl->m_bih);
	}

	copy_images(c, q, scene_cl);
}

void *cldata_create_scene(	cl_context c, cl_command_queue q,
				const scene_t *scene)
{
	scene_cl_t *scene_cl = malloc(sizeof(scene_cl_t));

	copy_scene(c, q, scene_cl, scene);

	clFinish(q);

	return scene_cl;
}
