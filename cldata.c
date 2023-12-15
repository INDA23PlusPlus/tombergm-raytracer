#include <string.h>
#include <CL/opencl.h>
#include "cldata.h"
#include "scene.h"

static int	n_bufs;
static void *	p_bufs[512];

static void *clmalloc(cl_context c, cl_command_queue q, size_t size)
{
	void *ptr = clSVMAlloc(c, CL_MEM_READ_ONLY, size, 0);

	clEnqueueSVMMap(q, CL_TRUE, CL_MAP_WRITE_INVALIDATE_REGION,
			ptr, size, 0, NULL, NULL);

	p_bufs[n_bufs++] = ptr;

	return ptr;
}

static void clunmap(cl_context c, cl_command_queue q, void *ptr)
{
	clEnqueueSVMUnmap(q, ptr, 0, NULL, NULL);
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

static void copy_tex(	cl_context c, cl_command_queue q,
			tex_cl_t *tex_cl, const tex_t *tex,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	tex_cl->w = tex->w;
	tex_cl->h = tex->h;

	if (tex->c != NULL)
	{
		tex_cl->c = clmalloc(c, q, tex->w * tex->h * 3);
		memcpy(tex_cl->c, tex->c, tex->w * tex->h * 3);
		clunmap(c, q, tex_cl->c);
	}
	else
	{
		tex_cl->c = NULL;
	}

	if (tex->n != NULL)
	{
		tex_cl->n = clmalloc(c, q, tex->w * tex->h * 3);
		memcpy(tex_cl->n, tex->n, tex->w * tex->h * 3);
		clunmap(c, q, tex_cl->n);
	}
	else
	{
		tex_cl->n = NULL;
	}

	if (tex->r != NULL)
	{
		tex_cl->r = clmalloc(c, q, tex->w * tex->h * 3);
		memcpy(tex_cl->r, tex->r, tex->w * tex->h * 3);
		clunmap(c, q, tex_cl->r);
	}
	else
	{
		tex_cl->r = NULL;
	}
}

static void copy_mat(	cl_context c, cl_command_queue q,
			mat_cl_t *mat_cl, const mat_t *mat,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	if (mat->tex != NULL)
	{
		mat_cl->tex = scene_cl->p_tex + (mat->tex - scene->p_tex);
	}
	else
	{
		mat_cl->tex = NULL;
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
		tri_cl->mat = scene_cl->p_mat + (tri->mat - scene->p_mat);
	}
	else
	{
		tri_cl->mat = NULL;
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

	sph_cl->mat = scene_cl->p_mat + (sph->mat - scene->p_mat);
}

static void copy_scene(	cl_context c, cl_command_queue q,
			scene_cl_t *scene_cl, const scene_t *scene)
{
	{
		scene_cl->n_tex = scene->n_tex;
		scene_cl->p_tex =
			clmalloc(c, q, sizeof(tex_cl_t) * scene->n_tex);

		for (int i = 0; i < scene->n_tex; i++)
		{
			copy_tex(	c, q,
					&scene_cl->p_tex[i],
					&scene->p_tex[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_tex);
	}

	{
		scene_cl->n_mat = scene->n_mat;
		scene_cl->p_mat =
			clmalloc(c, q, sizeof(mat_cl_t) * scene->n_mat);

		for (int i = 0; i < scene->n_mat; i++)
		{
			copy_mat(	c, q,
					&scene_cl->p_mat[i],
					&scene->p_mat[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_mat);
	}

	{
		scene_cl->n_tri = scene->n_tri;
		scene_cl->p_tri =
			clmalloc(c, q, sizeof(tri_cl_t) * scene->n_tri);

		for (int i = 0; i < scene->n_tri; i++)
		{
			copy_tri(	c, q,
					&scene_cl->p_tri[i],
					&scene->p_tri[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_tri);
	}

	{
		scene_cl->n_sph = scene->n_sph;
		scene_cl->p_sph =
			clmalloc(c, q, sizeof(sph_cl_t) * scene->n_sph);

		for (int i = 0; i < scene->n_sph; i++)
		{
			copy_sph(	c, q,
					&scene_cl->p_sph[i],
					&scene->p_sph[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_sph);
	}

	{
		scene_cl->n_sph_light = scene->n_sph_light;
		scene_cl->p_sph_light =
			clmalloc(c, q, sizeof(sph_cl_t) * scene->n_sph_light);

		for (int i = 0; i < scene->n_sph_light; i++)
		{
			copy_sph(	c, q,
					&scene_cl->p_sph_light[i],
					&scene->p_sph_light[i],
					scene_cl, scene);
		}

		clunmap(c, q, scene_cl->p_sph_light);
	}
}

void *cldata_create_scene(	cl_context c, cl_command_queue q,
				const scene_t *scene)
{
	scene_cl_t *scene_cl = clmalloc(c, q, sizeof(scene_cl_t));

	copy_scene(c, q, scene_cl, scene);

	clunmap(c, q, scene_cl);

	clFinish(q);

	return scene_cl;
}

void cldata_set_kernel_bufs(cl_kernel k)
{
	clSetKernelExecInfo(	k, CL_KERNEL_EXEC_INFO_SVM_PTRS,
				sizeof(void *) * n_bufs, p_bufs);
}
