#ifndef PLAYER_H
#define PLAYER_H

#include "game001/game001.h"
#include "game001/misc.h"
#include "entity.h"

struct Player {
	Spritesheet sprite;
	struct Entity entity;
	int honkOffst;

	byte lastHonk;
	byte flipped;
	byte frame[2];
};

void playerUpdate(void);

#endif
