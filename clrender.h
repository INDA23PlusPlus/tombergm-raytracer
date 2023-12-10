#ifndef CLRENDER_H
#define CLRENDER_H

#include "cam.h"
#include "scene.h"
#include "vec.h"
#include "vp.h"

void clrender_init(unsigned char *pb, const vp_t *vp, const scene_t *scene);
void clrender_commit(	cam_t *cam, vp_t *vp,
			unsigned char *pb, vec3_t *sb,
			int sn);
void clrender_wait(void);

#endif
