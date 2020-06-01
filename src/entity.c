#include "main.h"

#include "entity.h"

struct Game game;

void entityUpdate(struct Entity *entity, 
		void (*collisionCallback)(struct Entity *entity, struct EntityCollision *data),
		void (*frameChangeCallback)(struct Entity *entity, int frame))
{
	struct Box *boxptr;
	struct EntityCollision cevent;
	byte groundCollision;
	int collisionSide;

	entity->frames++;

	if(entity->input[GameInputLeft]) {
		if(entity->velocity[0] == 0)
			entity->frameChange = 1;
		entity->velocity[0] = -entity->speed;
		//entity->flipped = 1;
		if(frameChangeCallback)
			frameChangeCallback(entity, EntityFrameFlipped);
	} else if(entity->input[GameInputRight]) {
		if(entity->velocity[0] == 0)
			entity->frameChange = 1;
		entity->velocity[0] = entity->speed;
		//entity->flipped = 0;
		if(frameChangeCallback)
			frameChangeCallback(entity, EntityFrameNotFlipped);
	} else {
		if(entity->velocity[0] != 0)
			entity->frameChange = 1;
		entity->velocity[0] = 0;
	}

	if(!entity->onGround) {
		entity->velocity[1] += entity->gravity;
	} else {
		if(entity->input[GameInputUp]) {
			entity->velocity[1] = entity->jump;
			if(frameChangeCallback)
				frameChangeCallback(entity, EntityFrameAir);
			entity->onGround = 0;
		} else {
			entity->velocity[1] = 1.0f;
		}
	}

	if(entity->onGround) {
		if(entity->velocity[0] && entity->velocity[1]) {
			if(entity->frames >= FRAMES_TILL_CHANGE_WALK || entity->frameChange) {
				entity->frames = 0;
				entity->frameChange = 0;
				if(frameChangeCallback)
					frameChangeCallback(entity, EntityFrameIdle);
			}
		} else {
			if(entity->frames >= FRAMES_TILL_CHANGE_IDLE || entity->frameChange) {
				entity->frames = 0;
				entity->frameChange = 0;
				if(frameChangeCallback)
					frameChangeCallback(entity, EntityFrameWalk);
			}
		}
	}

	entity->pos[0] += entity->velocity[0];
	entity->pos[1] += entity->velocity[1];

	groundCollision = 0;

	if(entity->pos[1] + entity->size[1] > game.winsize[1]) {
		if(collisionCallback) {
			cevent.side = CollisionTop;
			cevent.gameBorder = 1;
			cevent.box = NULL;
			collisionCallback(entity, &cevent);
		}
		entity->pos[1] = game.winsize[1] - entity->size[1];

		if(!entity->onGround) {
			entity->onGround = 1;
			entity->frameChange = 1;
		}
		groundCollision = 1;
	}

	if(entity->pos[0] < 0) {
		if(collisionCallback) {
			cevent.side = CollisionRight;
			cevent.gameBorder = 1;
			cevent.box = NULL;
			collisionCallback(entity, &cevent);
		}
		entity->pos[0] = 0;
	} else if(entity->pos[0] + entity->size[0] > game.winsize[0]) {
		if(collisionCallback) {
			cevent.side = CollisionLeft;
			cevent.gameBorder = 1;
			cevent.box = NULL;
			collisionCallback(entity, &cevent);
		}
		entity->pos[0] = game.winsize[0] - entity->size[0];
	}

	//collide with sides first to avoid colliding with top or bottom of something you cant reach
	for(boxptr = game.boxes; boxptr != NULL; boxptr = boxptr->next) {
		collisionSide = entityCollideRect(entity, boxptr->pos[0], boxptr->pos[1], boxptr->size[0], boxptr->size[1]);
		if(collisionCallback && collisionSide) {
			cevent.side = collisionSide;
			cevent.gameBorder = 0;
			cevent.box = boxptr;
			collisionCallback(entity, &cevent);
		}
		switch(collisionSide) {
			case CollisionLeft:
				entity->pos[0] = boxptr->pos[0] + boxptr->size[0];
				break;
			case CollisionRight:
				entity->pos[0] = boxptr->pos[0] - entity->size[0];
				break;
		}
	}
	
	for(boxptr = game.boxes; boxptr != NULL; boxptr = boxptr->next) {
		collisionSide = entityCollideRect(entity, boxptr->pos[0], boxptr->pos[1], boxptr->size[0], boxptr->size[1]);
		if(collisionCallback && collisionSide) {
			cevent.side = collisionSide;
			cevent.gameBorder = 0;
			cevent.box = boxptr;
			collisionCallback(entity, &cevent);
		}
		switch(collisionSide) {
			case CollisionTop:
				entity->pos[1] = boxptr->pos[1] - entity->size[1];
				if(!entity->onGround) {
					entity->onGround = 1;
					entity->frameChange = 1;
				}
				groundCollision = 1;
				break;
			case CollisionBottom:
				entity->pos[1] = boxptr->pos[1] + boxptr->size[1];
				entity->velocity[1] = 0;
				break;
		}
	}

	entity->onGround = groundCollision;

	entity->lastpos[0] = entity->pos[0];
	entity->lastpos[1] = entity->pos[1];
}

int entityCollideRect(struct Entity *entity, float x, float y, int width, int height)
{
	int collision = CollisionNone;

	if(entity->pos[0] + entity->size[0] > x) {
		//right
		if(entity->lastpos[0] + entity->size[0] <= x)
			collision = CollisionRight;
	} else {
		return CollisionNone;
	}
	
	if(entity->pos[0] < x + width) {
		//left
		if(entity->lastpos[0] >= x + width)
			collision = CollisionLeft;
	} else {
		return CollisionNone;
	}
	
	if(entity->pos[1] < y + height) {
		//bottom
		if(entity->lastpos[1] >= y + height)
			collision = CollisionBottom;
	} else {
		return CollisionNone;
	}
	
	if(entity->pos[1] + entity->size[1] > y) {
		//top
		if(entity->lastpos[1] + entity->size[1] <= y)
			collision = CollisionTop;
	} else {
		return CollisionNone;
	}
		
	return collision;
}
