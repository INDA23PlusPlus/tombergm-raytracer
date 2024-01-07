#ifndef RT_H
#define RT_H

#include <stdio.h>

typedef enum
{
	RT_NULL,
	RT_ST,
	RT_MT,
	RT_CL,
} rt_if_t;

extern int	mouse_x;
extern int	mouse_y;
extern int	mouse_l;
extern int	mouse_r;
extern int	render_x;
extern int	render_y;

#ifdef DEBUG

#define dbgpx(fmt, ...) \
({ \
	if (mouse_l && mouse_x == render_x && mouse_y == render_y) \
	{ \
		fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__); \
	} \
})

#define dbgbrk() \
({ \
	__asm__ volatile ("int3;"); \
})

#define brkpx() \
({ \
	if (mouse_l && mouse_x == render_x && mouse_y == render_y) \
	{ \
		dbgbrk(); \
	} \
})

#else

#define dbgpx(fmt, ...)
#define dbgbrk()
#define brkpx()

#endif

#endif
