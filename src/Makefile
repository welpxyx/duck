PKG_CONFIG=pkg-config
CFLAGS = -I. \
	 `$(PKG_CONFIG) --cflags freetype2`
LDFLAGS = -lglfw -lGL -lGLEW -lm \
	  `$(PKG_CONFIG) -libs freetype2`
OPTIONS = -O2 -Wall
OBJ = main.o player.o crab.o entity.o
GAME001OBJ = `find game001 -regex ".*\.o"`
CC = gcc

all: duck

.c.o:
	$(CC) $(OPTIONS) $(CFLAGS) -c $<

main.o: game001/game001.h game001/misc.h main.h player.h
player.o: game001/game001.h game001/misc.h main.h player.h entity.h
crab.o: game001/game001.h game001/misc.h main.h player.h entity.h
entity.o: game001/misc.h main.h

game001:
	cd game001 && make

duck: $(OBJ) game001
	$(CC) -o $@ $(GAME001OBJ) $(OBJ) $(LDFLAGS) $(OPTIONS)

clean:
	rm $(OBJ)
	rm duck
	cd game001 && make clean

.PHONY: game001
