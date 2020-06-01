#include "main.h"
#include <stdio.h>

#include "crab.h"

struct Game game;
void crabFrameChangeCallback(struct Entity *entity, int frame);
void crabCollisionCallback(struct Entity *entity, struct EntityCollision *data);
int crabWalkTest(struct Crab *crab, byte side);

int crabframe = -1;
int crabcollision = 0;

void crabUpdate(struct Crab *crab)
{
	entityUpdate(&crab->entity, crabCollisionCallback, crabFrameChangeCallback);

	if(crabframe >= 0) {
		switch(crabframe) {
			case EntityFrameFlipped:
				crab->flipped = 1;
				break;
			case EntityFrameNotFlipped:
				crab->flipped = 0;
				break;
			case EntityFrameAir:
				crab->frame[1] = 0;
				break;
			case EntityFrameWalk:
				crab->frame[1] = (crab->frame[1] == 3)?4:3;
				break;
			case EntityFrameIdle:
				if(++crab->frame[1] > 2)
					crab->frame[1] = 0;
				break;
		}
		crabframe = -1;
	}

	if(!crab->entity.onGround)
		crab->dir = 2;
	else {
		if(crab->dir == 2)
			crab->dir = 0;
		if(crabcollision) {
			switch(crabcollision) {
				case 1:
					if(!crabWalkTest(crab, crab->dir?0:1))
						crab->dir = !crab->dir;
					break;
				case 2:
					crab->dir = !crab->dir;
			}
			crabcollision = 0;
		}
	}

	switch(crab->dir) {
		case 0:
			crab->entity.input[GameInputLeft] = 1;
			crab->entity.input[GameInputRight] = 0;
			break;
		case 1:
			crab->entity.input[GameInputLeft] = 0;
			crab->entity.input[GameInputRight] = 1;
			break;
		case 2:
			crab->entity.input[GameInputLeft] = 0;
			crab->entity.input[GameInputRight] = 0;
			break;
	}
}

void crabInit(struct Crab *crab)
{
	int i;
	
	crab->entity.lastpos[0] = crab->entity.pos[0] = 440.0f;
	crab->entity.lastpos[1] = crab->entity.pos[1] = 0.0f;
	crab->entity.size[0] = 64;
	crab->entity.size[1] = 64;
	crab->entity.velocity[0] = 0.0f;
	crab->entity.velocity[1] = 0.0f;
	crab->entity.speed = 1.0;
	crab->entity.gravity = 0.025f;
	crab->entity.jump = -3.0f;
	crab->entity.onGround = 0;
	crab->frame[0] = 0;
	crab->frame[1] = 0;
	crab->flipped = 0;
	crab->entity.frames = 0;
	crab->entity.frameChange = 0;
	crab->entity.frameAlt = 0;

	for(i = 0; i < GAME_INPUT_SIZE; i++)
		crab->entity.input[i] = 0;
}

void crabListUpdate(struct CrabList *list)
{
	while(list != NULL) {
		crabUpdate(&list->crab);
		list = list->next;
	}
}
		
void crabListRender(struct CrabList *list)
{
	while(list != NULL) {
		drawSpritesheet(&game.renderer, &game.crabSprite, list->crab.entity.pos[0], list->crab.entity.pos[1], list->crab.entity.size[0], list->crab.entity.size[1], 0, list->crab.frame[0], list->crab.frame[1], list->crab.flipped);
		list = list->next;
	}
}

void crabFrameChangeCallback(struct Entity *entity, int frame)
{
	(void) entity;
	crabframe = frame;
}

void crabCollisionCallback(struct Entity *entity, struct EntityCollision *data)
{
	if(entity->onGround) {
		if(data->side == CollisionLeft || data->side == CollisionRight)
			crabcollision = 2;
		else if(data->gameBorder)
			crabcollision = 3;
		else
			crabcollision = 1;
	}
}

int crabWalkTest(struct Crab *crab, byte side)
{
	struct Box *boxptr;

	float top1, bottom1, left1, right1;
	float top2, bottom2, left2, right2;

	top1 = crab->entity.pos[1] + crab->entity.gravity;
	bottom1 = top1 + crab->entity.size[1];
	left1 = crab->entity.pos[0] + (float)((side)?(-crab->entity.size[0]):(crab->entity.size[0]));
	right1 = left1 + crab->entity.size[0];

	for(boxptr = game.boxes; boxptr != NULL; boxptr = boxptr->next) {
		top2 = boxptr->pos[1];
		bottom2 = top2 + boxptr->size[1];
		left2 = boxptr->pos[0];
		right2 = left2 + boxptr->size[0];

		if(right1 > left2 && left1 < right2 && bottom1 > top2 && top1 < bottom2 
				&& top2 >= crab->entity.pos[1] + crab->entity.size[1]) {
			return 1;
		}
	}

	return 0;
}
