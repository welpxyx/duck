PKG_CONFIG=pkg-config
CFLAGS = -I. \
	 `$(PKG_CONFIG) --cflags freetype2`
LDFLAGS = -lglfw -lGL -lGLEW -lm \
	  `$(PKG_CONFIG) -libs freetype2`
OPTIONS = -O2 -Wall
OBJ = window.o graphics.o draw.o load.o matrix.o misc.o update.o cleanup.o
CC = gcc

all: game001

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<

window.o: game001.h misc.h
graphics.o: game001.h misc.h
misc.o: game001.h misc.h
matrix.o: game001.h
draw.o: game001.h misc.h stb_image.h
load.o: game001.h misc.h
update.o: game001.h
cleanup.o: game001.h

game001: $(OBJ)

clean:
	rm $(OBJ)

.PHONY: game001
