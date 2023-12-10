#ifndef RT_H
#define RT_H

#include <stdio.h>

extern int	mouse_x;
extern int	mouse_y;
extern int	mouse_l;
extern int	mouse_r;
extern int	render_x;
extern int	render_y;

#define dbgpx(fmt, ...) \
({ \
	if (1 && mouse_l && mouse_x == render_x && mouse_y == render_y) \
	{ \
		fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__); \
	} \
})

#endif
