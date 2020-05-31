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

struct Box {
	float pos[2];
	int size[2];

	struct Box *next;
};

struct Game {
	int input[GAME_INPUT_SIZE];
	int winsize[2];
	struct Box *boxes;
};
