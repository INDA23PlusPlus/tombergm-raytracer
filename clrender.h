#ifndef CLRENDER_H
#define CLRENDER_H

#include "cam.h"
#include "scene.h"
#include "vec.h"
#include "vp.h"

int	clrender_init(scene_t *scene, unsigned char *pb, const vp_t *vp);
void	clrender_dstr(void);
void	clrender_commit(scene_t *scene, cam_t *cam, vp_t *vp,
			unsigned char *pb, vec3_t *sb,
			int sn);
void	clrender_wait(void);

#endif
