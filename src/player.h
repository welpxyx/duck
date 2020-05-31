#include "game001/game001.h"
#include "game001/misc.h"

struct Player {
	Spritesheet sprite;

	float pos[2];
	float lastpos[2];
	float velocity[2];
	float speed, gravity;
	float jump;
	int size[2];
	int frames;

	byte onGround, flipped;
	byte frame[2];
	byte frameChange;
	byte doubleJump;
};

void playerUpdate(void);
int playerCollideRect(float x, float y, int width, int height);
