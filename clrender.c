#include <CL/cl.h>
#include <stdio.h>
#include <CL/opencl.h>
#include "cam.h"
#include "cldata.h"
#include "clrender.h"
#include "scene.h"
#include "vp.h"

#define PLAT	0
#define DEV	0

static cl_context	ctxt;
static cl_command_queue	queue;
static cl_program	prog;
static cl_kernel	kern;
static cl_event		event[3];

static int		buf_w;
static int		buf_h;

static cam_cl_t		cam_cl;
static scene_cl_t *	scene_cl;
static cl_int		sn_cl;

static cl_mem		mem_pb;
static cl_mem		mem_sb;
static cl_mem		mem_cam;

static const char *	src =
{
#include "obj/cl.c.inc"
};

int clrender_init(scene_t *scene, unsigned char *pb, const vp_t *vp)
{
	cl_int			res;
	cl_platform_id		plat[PLAT + 1];
	cl_context_properties	prop[3];
	cl_uint			nplat	= 0;
	cl_device_id		dev[DEV + 1];
	cl_uint			ndev	= 0;

	clGetPlatformIDs(PLAT + 1, plat, &nplat);

	if (nplat < PLAT + 1)
	{
		return -1;
	}

	clGetDeviceIDs(plat[PLAT], CL_DEVICE_TYPE_ALL, DEV + 1, dev, &ndev);

	if (ndev < DEV + 1)
	{
		return -1;
	}

	{
		char n[256];
		char v[256];

		clGetDeviceInfo(dev[DEV], CL_DEVICE_NAME,
				sizeof(n), n, NULL);
		clGetDeviceInfo(dev[DEV], CL_DEVICE_VERSION,
				sizeof(v), v, NULL);

		fprintf(stderr, "Using OpenCL    : %s, %s\n", n, v);
	}

	prop[0] = CL_CONTEXT_PLATFORM;
	prop[1] = (cl_context_properties) plat[PLAT];
	prop[2] = 0;

	ctxt = clCreateContext(prop, 1, &dev[DEV], NULL, NULL, NULL);

	if (ctxt == NULL)
	{
		return -1;
	}

	queue = clCreateCommandQueueWithProperties(ctxt, dev[DEV], NULL, NULL);

	if (queue == NULL)
	{
		clrender_dstr();

		return -1;
	}

	prog = clCreateProgramWithSource(ctxt, 1, &src, NULL, NULL);

	if (prog == NULL)
	{
		clrender_dstr();

		return -1;
	}

	res = clBuildProgram(prog, 0, NULL, NULL, NULL, NULL);

	{
		char log[1024 * 256];
		size_t log_size;

		clGetProgramBuildInfo(	prog, dev[DEV], CL_PROGRAM_BUILD_LOG,
					sizeof(log), log, &log_size);

		if (log_size > 2)
		{
			fprintf(stderr, "OpenCL build log:\n%s", log);
		}
	}

	if (res != CL_SUCCESS)
	{
		clrender_dstr();

		return -1;
	}

	kern = clCreateKernel(prog, "render", NULL);

	if (kern == NULL)
	{
		clrender_dstr();

		return -1;
	}

	buf_w = vp->w;
	buf_h = vp->h;

	scene_cl = cldata_create_scene(ctxt, queue, scene);

	mem_pb		= clmalloc_ext(	ctxt, CL_MEM_WRITE_ONLY,
					buf_w * buf_h * 4,
					pb);
	mem_sb		= clmalloc_ext(	ctxt, CL_MEM_READ_WRITE,
					buf_w * buf_h * sizeof(cl_float3),
					NULL);
	mem_cam		= clmalloc_ext(	ctxt, CL_MEM_READ_ONLY,
					sizeof(cam_cl),
					&cam_cl);

	clSetKernelArg(kern, 0, sizeof(mem_pb), &mem_pb);
	clSetKernelArg(kern, 1, sizeof(mem_sb), &mem_sb);
	clSetKernelArg(kern, 2, sizeof(mem_cam), &mem_cam);
	clSetKernelArg(kern, 3, sizeof(scene_cl->n_tex), &scene_cl->n_tex);
	clSetKernelArg(kern, 4, sizeof(scene_cl->m_tex), &scene_cl->m_tex);
	clSetKernelArg(kern, 5, sizeof(scene_cl->n_mat), &scene_cl->n_mat);
	clSetKernelArg(kern, 6, sizeof(scene_cl->m_mat), &scene_cl->m_mat);
	clSetKernelArg(kern, 7, sizeof(scene_cl->n_tri), &scene_cl->n_tri);
	clSetKernelArg(kern, 8, sizeof(scene_cl->m_tri), &scene_cl->m_tri);
	clSetKernelArg(kern, 9, sizeof(scene_cl->n_sph), &scene_cl->n_sph);
	clSetKernelArg(kern, 10, sizeof(scene_cl->m_sph), &scene_cl->m_sph);
	clSetKernelArg(kern, 11, sizeof(scene_cl->n_prim), &scene_cl->n_prim);
	clSetKernelArg(kern, 12, sizeof(scene_cl->m_prim), &scene_cl->m_prim);
	clSetKernelArg(kern, 13, sizeof(scene_cl->n_bih), &scene_cl->n_bih);
	clSetKernelArg(kern, 14, sizeof(scene_cl->m_bih), &scene_cl->m_bih);
	clSetKernelArg(kern, 15, sizeof(scene_cl->m_img), &scene_cl->m_img);
	clSetKernelArg(kern, 16, sizeof(scene_cl->m_box), &scene_cl->m_box);

	cldata_show_mu();

	return 0;
}

void clrender_dstr(void)
{
	if (kern != NULL)
	{
		clReleaseKernel(kern);

		kern = NULL;
	}

	if (scene_cl != NULL)
	{
		cldata_dstr_scene(scene_cl);

		scene_cl = NULL;
	}

	if (mem_pb != NULL)
	{
		clReleaseMemObject(mem_pb);

		mem_pb = NULL;
	}

	if (mem_sb != NULL)
	{
		clReleaseMemObject(mem_sb);

		mem_sb = NULL;
	}

	if (mem_cam != NULL)
	{
		clReleaseMemObject(mem_cam);

		mem_cam = NULL;
	}

	if (prog != NULL)
	{
		clReleaseProgram(prog);

		prog = NULL;
	}

	if (queue != NULL)
	{
		clReleaseCommandQueue(queue);

		queue = NULL;
	}

	if (ctxt != NULL)
	{
		clReleaseContext(ctxt);

		ctxt = NULL;
	}
}

void clrender_commit(	scene_t *scene, cam_t *cam, vp_t *vp,
			unsigned char *pb, vec3_t *sb,
			int sn)
{
	{
		clEnqueueMapBuffer(	queue, mem_cam, CL_TRUE,
					CL_MAP_WRITE_INVALIDATE_REGION,
					0, sizeof(cam_cl),
					0, NULL, NULL, NULL);

		cam_cl.p.x	= cam->p.x;
		cam_cl.p.y	= cam->p.y;
		cam_cl.p.z	= cam->p.z;
		cam_cl.uv.x	= cam->uv.x;
		cam_cl.uv.y	= cam->uv.y;
		cam_cl.uv.z	= cam->uv.z;
		cam_cl.fv.x	= cam->fv.x;
		cam_cl.fv.y	= cam->fv.y;
		cam_cl.fv.z	= cam->fv.z;
		cam_cl.l	= cam->l;
		cam_cl.r	= cam->r;
		cam_cl.t	= cam->t;
		cam_cl.b	= cam->b;
		cam_cl.n	= cam->n;
		cam_cl.ap	= cam->ap;

		clEnqueueUnmapMemObject(queue,
					mem_cam, &cam_cl,
					0, NULL, &event[0]);
	}

	{
		sn_cl = sn;

		clSetKernelArg(kern, 17, sizeof(sn_cl), &sn_cl);
	}

	{
		cl_uint sn_rand = rand();

		clSetKernelArg(kern, 18, sizeof(sn_rand), &sn_rand);
	}

	{
		size_t gw[2] = { buf_w, buf_h };

		clEnqueueNDRangeKernel(	queue, kern,
					2, NULL, gw, NULL,
					1, &event[0], &event[1]);

		clEnqueueMapBuffer(	queue, mem_pb, CL_FALSE,
					CL_MAP_READ,
					0, buf_w * buf_h * 4,
					1, &event[1], &event[2], NULL);
	}
}

void clrender_wait(void)
{
	clWaitForEvents(1, &event[2]);
}
