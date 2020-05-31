#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "game001/misc.h"

#include "game001/game001.h"

#define GAME_INPUT_SIZE 5
#define FRAMES_TILL_CHANGE_IDLE 100
#define FRAMES_TILL_CHANGE_WALK 50
#define MS_PER_UPDATE 0.003

enum GameInput {
	GameInputUp, GameInputDown, GameInputLeft, GameInputRight, GameInputDoubleJump
};

enum Collision {
	CollisionNone, CollisionTop, CollisionBottom, CollisionLeft, CollisionRight
};

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
} player;

struct Box {
	float pos[2];
	int size[2];

	struct Box *next;
};

struct Game {
	int input[GAME_INPUT_SIZE];
	int winsize[2];
	struct Box *boxes;
} game;

Cleanup cleanup;
extern char *program_name;

void varInit(void);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void updatePlayer(void);
int playerCollideRect(float x, float y, int width, int height);

int main(int argc, char **argv) {
	int i;
	int loop = 1;
	Renderer renderer;
	Texture text;
	Texture boxtex;
	Font bestfont;
	struct Box *boxptr;
	float timeNow, timeLast, timeDiff;

	program_name = argv[0];

	game.winsize[0] = 640;
	game.winsize[1] = 480;
	
	cleanupInit();

	createWindow(&renderer.window, "ducks", game.winsize[0], game.winsize[1]);

	varInit();
	rendererInit(&renderer);
	createFont(&renderer, &bestfont, "resources/Hack-Regular.ttf", 24);
	createTexture(&player.sprite.tex, "resources/duck.png", NULL);
	createSpritesheet(&renderer, &player.sprite, 1, 5);
	createTexture(&boxtex, "resources/rock.png", NULL);
	drawFontTex(&renderer, &text, &bestfont, "duck for president 2020", 255, 0, 0);
	
	float boxRects[][4] = {
		{400, game.winsize[1] - 150, 50, 50}
	};

	boxptr = (struct Box *)xmalloc(sizeof(struct Box));
	game.boxes = boxptr;

	for(i = 0; i < ARRSIZE(boxRects); i++) {
		boxptr->pos[0] = boxRects[i][0];
		boxptr->pos[1] = boxRects[i][1];
		boxptr->size[0] = (float)boxRects[i][2];
		boxptr->size[1] = (float)boxRects[i][3];
		
		if(i < ARRSIZE(boxRects) - 1) {
			boxptr->next = (struct Box *)xmalloc(sizeof(struct Box));
			boxptr = boxptr->next;
		} else {
			boxptr->next = NULL;
		}
	}



	glfwSetKeyCallback(renderer.window.obj, key_callback);

	timeLast = glfwGetTime();

	timeDiff = 0;

	while(loop) {
		timeNow = glfwGetTime();
		timeDiff += timeNow - timeLast;
		while(timeDiff >= MS_PER_UPDATE) {
			//update things
			updatePlayer();

			timeDiff -= MS_PER_UPDATE;
		}
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//render thingys
		drawTexture(&renderer, &text, 0, 0, text.width, text.height, 0);
		drawSpritesheet(&renderer, &player.sprite, player.pos[0], player.pos[1], player.size[0], player.size[1], 0, player.frame[0], player.frame[1], player.flipped);

		for(boxptr = game.boxes; boxptr != NULL; boxptr = boxptr->next) {
			drawTexture(&renderer, &boxtex, boxptr->pos[0], boxptr->pos[1], boxptr->size[0], boxptr->size[1], 0);
		}

		glfwSwapBuffers(renderer.window.obj);
		glfwPollEvents();

		if(glfwWindowShouldClose(renderer.window.obj))
			loop = 0;

		timeLast = timeNow;
	}

	boxptr = game.boxes;

	while(boxptr != NULL) {
		game.boxes = boxptr;
		boxptr = boxptr->next;
		cleanupAddAllocated((void **)&game.boxes, 1);
	}

	cleanupDestroy();
	glfwTerminate();

	return 0;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int keystate = ((action == GLFW_PRESS)?1:0);

	if(action == GLFW_PRESS || action == GLFW_RELEASE) {
		switch(key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				return;
			case GLFW_KEY_W:
			case GLFW_KEY_UP:
				game.input[GameInputUp] = keystate;
				if(keystate == GLFW_PRESS)
					game.input[GameInputDoubleJump] = keystate;
				break;
			case GLFW_KEY_S:
			case GLFW_KEY_DOWN:
				game.input[GameInputDown] = keystate;
				break;
			case GLFW_KEY_A:
			case GLFW_KEY_LEFT:
				game.input[GameInputLeft] = keystate;
				break;
			case GLFW_KEY_D:
			case GLFW_KEY_RIGHT:
				game.input[GameInputRight] = keystate;
				break;
		}
	}
}

void updatePlayer()
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

void varInit(void)
{
	player.lastpos[0] = player.pos[0] = 0.0f;
	player.lastpos[1] = player.pos[1] = 0.0f;
	player.size[0] = 64;
	player.size[1] = 64;
	player.velocity[0] = 0.0f;
	player.velocity[1] = 0.0f;
	player.speed = 1.0;
	player.gravity = 0.025f;
	player.jump = -3.0f;
	player.onGround = 0;
	player.frame[0] = 0;
	player.frame[1] = 0;
	player.flipped = 0;
	player.frames = 0;
	player.frameChange = 0;
	player.doubleJump = 1;
}
