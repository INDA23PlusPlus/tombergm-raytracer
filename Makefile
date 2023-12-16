PROG		= rt
SRCDIR		= ./
SRC		= cldata.c clrender.c main.c ray.c render.c scene.c sph.c stb_image.c tex.c tri.c vec.c
CLSRCDIR	= ./cl/
CLSRC		= main.c ray.c render.c sph.c tex.c tri.c vec.c
OBJDIR		= ./obj/
CPPFLAGS	= -DCL_TARGET_OPENCL_VERSION=200
ifeq ($(DEBUG),1)
CPPFLAGS	+= -DDEBUG
CFLAGS		+= -Og -g -Wall
else
CPPFLAGS	+=
CFLAGS		+= -O2 -g -Wall
endif
LDFLAGS		=
ifeq ($(shell uname -s | grep -o _NT-),_NT-)
LDLIBS		= -lfreeglut -lopengl32 -lglew32 -lOpenCL
else
LDLIBS		= -lpthread -lglut -lGL -lGLEW -lOpenCL -lm
endif

OBJ		= $(SRC:%.c=$(OBJDIR)%.o)
DEP		= $(OBJ:%=%.d)

.PHONY: all
all: $(PROG)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -f $(PROG)

$(OBJDIR)render.cl.h: $(CLSRC:%=$(CLSRCDIR)%) | $(OBJDIR)
	cat $(^) >$(@)

$(OBJDIR)render.cl.c: $(OBJDIR)render.cl.h | $(OBJDIR)
	$(CPP) -I$(CLSRCDIR) -MMD -MP -MT $(@) -MF $(@:%=%.d) $(<) | \
	sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/^/"/g' -e 's/$$/\\n"/g' >$(@)

-include $(OBJDIR)render.cl.c.d

$(OBJDIR)clrender.o: $(OBJDIR)render.cl.c

$(OBJDIR):
	mkdir -p $(@)

$(OBJ): $(OBJDIR)%.o: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) -c -o $(@) -MMD -MP -MF $(@:%=%.d) $(CPPFLAGS) $(CFLAGS) $(<)

-include $(DEP)

$(PROG): $(OBJ)
	$(CC) -o $(@) $(CFLAGS) $(LDFLAGS) $(^) $(LDLIBS)
