PROG		= rt
SRCDIR		= ./
SRC		= bih.c box.c cldata.c clrender.c main.c prim.c ray.c render.c scene.c sph.c stb_image.c tex.c tri.c vec.c vector.c
OBJDIR		= ./obj/
CPPFLAGS	= -DCL_TARGET_OPENCL_VERSION=200
CFLAGS		=
LDFLAGS		=

ifeq ($(DEBUG),1)
CPPFLAGS	+= -DDEBUG
CLCPPFLAGS	+=
CFLAGS		+= -Og -g -Wall
else
CPPFLAGS	+=
CLCPPFLAGS	+= -P
CFLAGS		+= -Ofast -flto=auto -fuse-linker-plugin -Wall
endif

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

$(OBJDIR):
	mkdir -p $(@)

-include $(OBJDIR)cl.c.d

$(OBJDIR)cl.c: $(SRCDIR)cl/main.c | $(OBJDIR)
	$(CPP) -o $(@) -MMD -MP -MT $(@) -MF $(@:%=%.d) $(CPPFLAGS) $(CLCPPFLAGS) $(<)

$(OBJDIR)cl.c.inc: $(OBJDIR)cl.c | $(OBJDIR)
	sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/^/"/g' -e 's/$$/\\n"/g' $(<) >$(@)

$(OBJDIR)clrender.o: $(OBJDIR)cl.c.inc

-include $(DEP)

$(OBJ): $(OBJDIR)%.o: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) -c -o $(@) -MMD -MP -MF $(@:%=%.d) $(CPPFLAGS) $(CFLAGS) $(<)

$(PROG): $(OBJ)
	$(CC) -o $(@) $(CFLAGS) $(LDFLAGS) $(^) $(LDLIBS)
