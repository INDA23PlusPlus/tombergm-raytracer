CFLAGS	= -Ofast -g -Wall
ifeq ($(shell uname -s | grep -o _NT-),_NT-)
LDLIBS	= -lfreeglut -lopengl32 -lglew32
else
LDLIBS	= -lglut -lGL -lGLEW -lm
endif

.PHONY: all
all: rt

.PHONY: clean
clean:
	rm -f rt

rt: main.c
	$(CC) -o $(@) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(^) $(LDLIBS)
