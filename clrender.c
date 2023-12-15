#include <stdio.h>
#include <CL/opencl.h>
#include "cam.h"
#include "cldata.h"
#include "scene.h"
#include "vp.h"

static cl_context	ctxt;
static cl_command_queue	queue;
static cl_program	prog;
static cl_kernel	kern;
static cl_event		event;

static int		buf_w;
static int		buf_h;

static cam_cl_t		cam_cl;
static scene_cl_t *	scene_cl;
static cl_int		sn_cl;

static cl_mem		mem_pb;
static cl_mem		mem_sb;
static cl_mem		mem_cam;
static cl_mem		mem_scene;

static const char *	src =
{
#include "obj/render.cl.c"
};

void clrender_init(unsigned char *pb, const vp_t *vp, const scene_t *scene)
{
	cl_platform_id		plat[2];
	cl_context_properties	prop[3];
  	cl_uint			nplat;
	cl_device_id		dev[2];
	cl_uint			ndev;

	buf_w = vp->w;
	buf_h = vp->h;

	clGetPlatformIDs(2, plat, &nplat);

	fprintf(stderr, "Platforms:\n");
	for (int i = 0; i < 2 && i < nplat; i++)
	{
		char v[64];
		char m[64];
		char n[64];

		clGetPlatformInfo(	plat[i], CL_PLATFORM_VERSION,
					sizeof(v), v, NULL);
		clGetPlatformInfo(	plat[i], CL_PLATFORM_VENDOR,
					sizeof(m), m, NULL);
		clGetPlatformInfo(	plat[i], CL_PLATFORM_NAME,
					sizeof(n), n, NULL);

		fprintf(stderr, "%i: %s - %s %s\n", i, m, n, v);
	}

	clGetDeviceIDs(plat[0], CL_DEVICE_TYPE_GPU, 2, dev, &ndev);

	fprintf(stderr, "Devices:\n");
	for (int i = 0; i < 2 && i < ndev; i++)
	{
		char v[64];
		char m[64];
		char n[64];

		clGetDeviceInfo(dev[i], CL_DEVICE_VERSION,
				sizeof(v), v, NULL);
		clGetDeviceInfo(dev[i], CL_DEVICE_VENDOR,
				sizeof(m), m, NULL);
		clGetDeviceInfo(dev[i], CL_DEVICE_NAME,
				sizeof(n), n, NULL);

		fprintf(stderr, "%i: %s - %s %s\n", i, m, n, v);
	}

	prop[0] = CL_CONTEXT_PLATFORM;
	prop[1] = (cl_context_properties) plat[0];
	prop[2] = 0;

	ctxt = clCreateContextFromType(	prop, CL_DEVICE_TYPE_GPU, NULL,
					NULL, NULL);

	queue = clCreateCommandQueueWithProperties(ctxt, dev[0], NULL, NULL);

	prog = clCreateProgramWithSource(ctxt, 1, &src, NULL, NULL);
	clBuildProgram(prog, 0, NULL, NULL, NULL, NULL);
	{
		char log[1024 * 16];

		clGetProgramBuildInfo(	prog, dev[0], CL_PROGRAM_BUILD_LOG,
					sizeof(log), log, NULL);

		printf("%s\n", log);
	}

	kern = clCreateKernel(prog, "render", NULL);

	scene_cl = cldata_create_scene(ctxt, queue, scene);

	mem_pb	= clCreateBuffer(	ctxt,
					CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
					buf_w * buf_h * 4, pb, NULL);
	mem_sb	= clCreateBuffer(	ctxt,
					CL_MEM_READ_WRITE,
					buf_w * buf_h * sizeof(cl_float3),
					NULL, NULL);
	mem_cam	= clCreateBuffer(	ctxt,
					CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
					sizeof(cam_cl), &cam_cl, NULL);
	mem_scene
		= clCreateBuffer(	ctxt,
					CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
					sizeof*(scene_cl), scene_cl, NULL);

	cldata_set_kernel_bufs(kern);

	clSetKernelArg(kern, 0, sizeof(mem_pb), &mem_pb);
	clSetKernelArg(kern, 1, sizeof(mem_sb), &mem_sb);
	clSetKernelArg(kern, 2, sizeof(mem_cam), &mem_cam);
	clSetKernelArg(kern, 3, sizeof(mem_scene), &mem_scene);
}

void clrender_commit(	cam_t *cam, vp_t *vp,
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

		clEnqueueUnmapMemObject(queue,
					mem_cam, &cam_cl,
					0, NULL,
					&event);
	}

	{
		sn_cl = sn;

		clSetKernelArg(kern, 4, sizeof(sn_cl), &sn_cl);
	}

	{
		cl_uint sn_rand = rand();

		clSetKernelArg(kern, 5, sizeof(sn_rand), &sn_rand);
	}

	{
		size_t gw[2] = { buf_w, buf_h };
		size_t lw[2] = { 1, 1 };

		clEnqueueNDRangeKernel(	queue, kern, 2, NULL,
					gw, lw, 1, &event, &event);

		clEnqueueMapBuffer(	queue, mem_pb, CL_FALSE,
					CL_MAP_READ,
					0, buf_w * buf_h * 4,
					1, &event, &event, NULL);
	}
}

void clrender_wait(void)
{
	clFinish(queue);
}
