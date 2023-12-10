#ifndef RENDER_H
#define RENDER_H

#include <pthread.h>
#include "cam.h"
#include "vp.h"

typedef struct
{
	pthread_mutex_t	mtx;
	pthread_cond_t	cnd;
	pthread_t	thrd;
	int		task_num;
	int		task_max;
	cam_t *		cam;
	vp_t *		vp;
	unsigned char *	pb;
	vec3_t *	sb;
	int		sn;
} render_task_t;

int	render_task_init(render_task_t *rt_list, int rt_num);
void	render_task_commit(render_task_t *rt_list, int rt_num,
				cam_t *cam, vp_t *vp,
				unsigned char *pb, vec3_t *sb, int sn);
void	render_task_wait(render_task_t *rt_list, int rt_num);
void	render_task_dstr(render_task_t *rt_list, int rt_num);
int	render(cam_t *cam, vp_t *vp, unsigned char *pb, vec3_t *sb, int sn);

#endif
