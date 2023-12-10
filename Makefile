PROG		= rt
OBJDIR		= ./obj/
SRC		= main.c ray.c render.c scene.c sph.c stb_image.c tex.c tri.c vec.c
ifeq ($(DEBUG),1)
CPPFLAGS	= -DDEBUG
CFLAGS		= -Og -g -Wall
else
CPPFLAGS	=
CFLAGS		= -Ofast -flto -g -Wall
endif
LDFLAGS		=
ifeq ($(shell uname -s | grep -o _NT-),_NT-)
LDLIBS		= -lfreeglut -lopengl32 -lglew32
else
LDLIBS		= -lpthread -lglut -lGL -lGLEW -lm
endif

OBJ		= $(SRC:%.c=$(OBJDIR)%.o)
DEP		= $(SRC:%.c=$(OBJDIR)%.d)

.PHONY: all
all: $(PROG)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -f $(PROG)

-include $(DEP)

$(OBJDIR):
	mkdir -p $(@)

$(OBJ): $(OBJDIR)%.o: %.c | $(OBJDIR)
	$(CC) -c -o $(@) -MMD -MP -MF $(@:%.o=%.d) $(CPPFLAGS) $(CFLAGS) $(<)

$(PROG): $(OBJ)
	$(CC) -o $(@) $(CFLAGS) $(LDFLAGS) $(^) $(LDLIBS)
