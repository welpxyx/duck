#include "main.h"

#include "player.h"

extern struct Player player;
struct Game game;

void playerUpdate()
{
	struct Box *boxptr;

	player.frames++;

	if(game.input[GameInputLeft]) {
		if(player.velocity[0] == 0)
			player.frameChange = 1;
		player.velocity[0] = -player.speed;
		player.flipped = 1;
	} else if(game.input[GameInputRight]) {
		if(player.velocity[0] == 0)
			player.frameChange = 1;
		player.velocity[0] = player.speed;
		player.flipped = 0;
	} else {
		if(player.velocity[0] != 0)
			player.frameChange = 1;
		player.velocity[0] = 0;
	}

	if(!player.onGround) {
		player.velocity[1] += player.gravity;
		if(game.input[GameInputDoubleJump]) {
			if(player.doubleJump) {
				player.velocity[1] = player.jump;
				player.doubleJump = 0;
			}
			game.input[GameInputDoubleJump] = 0;
		}
	} else {
		if(game.input[GameInputUp]) {
			player.velocity[1] = player.jump;
			player.onGround = 0;
			game.input[GameInputDoubleJump] = 0;
		} else {
			player.velocity[1] = 1.0f;
		}
	}

	if(game.input[GameInputDown]) {
		player.frame[1] = 4;
		player.frameChange = 1;
	} else if(player.velocity[0] && player.velocity[1]) {
		if(player.frames >= FRAMES_TILL_CHANGE_WALK || player.frameChange) {
			player.frames = 0;
			player.frameChange = 0;
			player.frame[1] = (player.frame[1] == 2)?3:2;
		}
	} else {
		if(player.frames >= FRAMES_TILL_CHANGE_IDLE || player.frameChange) {
			player.frames = 0;
			player.frameChange = 0;
			player.frame[1] = (player.frame[1] == 0)?1:0;
		}
	}

	player.pos[0] += player.velocity[0];
	player.pos[1] += player.velocity[1];

	player.onGround = 0;

	if(player.pos[1] + player.size[1] > game.winsize[1]) {
		player.pos[1] = game.winsize[1] - player.size[1];
		player.onGround = 1;
		player.doubleJump = 1;
	}

	if(player.pos[0] < 0)
		player.pos[0] = 0;
	else if(player.pos[0] + player.size[0] > game.winsize[0])
		player.pos[0] = game.winsize[0] - player.size[0];

	//collide with sides first to avoid colliding with top or bottom of something you cant reach
	for(boxptr = game.boxes; boxptr != NULL; boxptr = boxptr->next) {
		switch(playerCollideRect(boxptr->pos[0], boxptr->pos[1], boxptr->size[0], boxptr->size[1])) {
			case CollisionLeft:
				player.pos[0] = boxptr->pos[0] + boxptr->size[0];
				break;
			case CollisionRight:
				player.pos[0] = boxptr->pos[0] - player.size[0];
				break;
		}
	}
	
	for(boxptr = game.boxes; boxptr != NULL; boxptr = boxptr->next) {
		switch(playerCollideRect(boxptr->pos[0], boxptr->pos[1], boxptr->size[0], boxptr->size[1])) {
			case CollisionTop:
				player.pos[1] = boxptr->pos[1] - player.size[1];
				player.onGround = 1;
				player.doubleJump = 1;
				break;
			case CollisionBottom:
				player.pos[1] = boxptr->pos[1] + boxptr->size[1];
				player.velocity[1] = 0;
				break;
		}
	}

	player.lastpos[0] = player.pos[0];
	player.lastpos[1] = player.pos[1];
}

int playerCollideRect(float x, float y, int width, int height)
{
	int collision = CollisionNone;

	if(player.pos[0] + player.size[0] > x) {
		//right
		if(player.lastpos[0] + player.size[0] <= x)
			collision = CollisionRight;
	} else {
		return CollisionNone;
	}
	
	if(player.pos[0] < x + width) {
		//left
		if(player.lastpos[0] >= x + width)
			collision = CollisionLeft;
	} else {
		return CollisionNone;
	}
	
	if(player.pos[1] < y + height) {
		//bottom
		if(player.lastpos[1] >= y + height)
			collision = CollisionBottom;
	} else {
		return CollisionNone;
	}
	
	if(player.pos[1] + player.size[1] > y) {
		//top
		if(player.lastpos[1] + player.size[1] <= y)
			collision = CollisionTop;
	} else {
		return CollisionNone;
	}
		
	return collision;
}
