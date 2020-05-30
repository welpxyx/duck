PKG_CONFIG=pkg-config
CFLAGS = -I. \
	 `$(PKG_CONFIG) --cflags freetype2`
LDFLAGS = -lglfw -lGL -lGLEW -lm \
	  `$(PKG_CONFIG) -libs freetype2`
OPTIONS = -O2 -Wall
OBJ = duck.o
GAME001OBJ = `find game001 -regex ".*\.o"`
CC = gcc

all: duck

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<

duck.o: game001/game001.h

game001:
	cd game001 && make

duck: $(OBJ) game001
	$(CC) -o $@ $(GAME001OBJ) $(OBJ) $(LDFLAGS) $(OPTIONS)

clean:
	rm $(OBJ)
	rm duck

.PHONY: game001
