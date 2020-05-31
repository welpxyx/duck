#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "game001/misc.h"
#include "game001/game001.h"
#include "player.h"

#include "main.h"

Cleanup cleanup;
struct Player player;
extern char *program_name;
extern struct Game game;

void varInit(void);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
			playerUpdate();

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
