#ifndef ENTITY_H
#define ENTITY_H

#include "game001/game001.h"
#include "game001/misc.h"

#include "main.h"

enum EntityFrame {
	EntityFrameIdle, EntityFrameWalk, EntityFrameAir, EntityFrameFlipped, EntityFrameNotFlipped
};

struct EntityCollision {
	byte side;
	byte gameBorder;
	struct Box *box;
};

struct Entity {
	float pos[2];
	float lastpos[2];
	float velocity[2];
	float speed, gravity;
	float jump;
	int size[2];
	int frames;

	byte onGround;
	//byte flipped;
	//byte frame[2];
	byte frameAlt;
	byte frameChange;
	byte doubleJump;

	byte input[GAME_INPUT_SIZE];
};

void entityUpdate(struct Entity *entity, 
		void (*collisionCallback)(struct Entity *entity, struct EntityCollision *data),
		void (*frameChangeCallback)(struct Entity *entity, int frame));

int entityCollideRect(struct Entity *entity, float x, float y, int width, int height);

#endif
