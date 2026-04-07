CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2 -Iinclude
LIBS    = -lGL -lGLU -lglut -lm

TARGET  = solar_system

# Arquivos dentro de src/
SRCS = src/main.c \
       src/obj_loader.c \
       src/textures.c \
       src/planets.c \
       src/render_utils.c \
       src/camera.c \
       src/comet.c \
       src/station.c \
       src/hud.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# regra genérica
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean