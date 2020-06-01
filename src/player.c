#include "main.h"
#include "entity.h"

#include "player.h"

extern struct Player player;
struct Game game;

void playerCollisionCallback(struct Entity *entity, struct EntityCollision *data);
void playerFrameChangeCallback(struct Entity *entity, int frame);

void playerUpdate()
{
	entityUpdate(&player.entity, playerCollisionCallback, playerFrameChangeCallback);
	if(game.input[GameInputDown]) {
		player.frame[1] = 4;
		player.lastHonk = 1;
	} else if(player.lastHonk) {
		player.lastHonk = 0;
		player.entity.frameChange = 1;

	}
}

void playerCollisionCallback(struct Entity *entity, struct EntityCollision *data)
{
	//printf("player event %d\n", data->gameBorder);
}

void playerFrameChangeCallback(struct Entity *entity, int frame)
{
	switch(frame) {
		case EntityFrameFlipped:
			player.flipped = 1;
			break;
		case EntityFrameNotFlipped:
			player.flipped = 0;
			break;
		case EntityFrameAir:
			player.frame[1] = 0;
			break;
		case EntityFrameIdle:
			player.frame[1] = (player.frame[1] == 2)?3:2;
			break;
		case EntityFrameWalk:
			player.frame[1] = (player.frame[1] == 0)?1:0;
			break;
	}
}
