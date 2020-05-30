#ifndef GAME001_H
#define GAME001_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "misc.h"

typedef struct {
	int width;
	int height;
	
	char *title;
	GLFWwindow *obj;
} Window;

typedef struct {
	GLuint id;
	int width;
	int height;
} Texture;

typedef struct {
	Texture tex;
	GLuint vao, flippedvao;

	float fwidth, fheight;
	int rows, cols;
} Spritesheet;

typedef struct {
	int size[2];
	int bearing[2];
	unsigned int advance;
} Character;

typedef struct {
	Character chars[128];
	Texture tex;
	unsigned int fwidth;
	unsigned int height;
	unsigned long spacing;
} Font;

typedef struct {
	GLuint vao, vbo;
	Texture tex;
	Font *font;
	char **text;
	int lines;
	int pos[2];
	int size[2];
} Dialogue;

typedef struct {
	float camera[3];	//x, y, zoom
	float view[16], proj[16];
	
	Window window;

	//GLuint *shaders;
	GLuint shaders[3];
	GLint *uniforms[3];
	GLuint ubos[2];

	int shadercount;
	
	GLuint quadvao, quadvbo, quadebo;
	GLuint textvao, textvbo, textfb;
} Renderer;

typedef struct {
	GLenum primitive;
	GLuint vao, vbo, ebo;
	float color[4];
	float size[2];
	int indices;
} Polygon;

typedef struct {
	int sizes[8];
	//glDeleteVertexArrays
	GLuint *vao;
	//glDeleteBuffers
	GLuint *vbo;
	//glDeleteFramebuffers
	GLuint *fbo;
	//glDeleteTextures
	GLuint *tex;
	//glDeleteProgram
	GLuint *program;
	//glfwDestroyWindow
	GLFWwindow **window;
	//free
	void **allocated;
	void ***allocated_p;
} Cleanup;

//window.c
Window *createWindow(Window *win, const char *title, int width, int height);

//graphics.c
void rendererInit(Renderer *renderer);
Dialogue *setupDialogue(const Renderer *renderer, Font *font, Dialogue *dialogue, int lines, const char **text, int r, int g, int b);

//matrix.c
float *mat4Multiply(const float *m1, const float *m2, float *dest);
float *mat4Identity(float *m);
float *mat4Translate(float *m, float x, float y);
float *mat4Scale(float *m, float x, float y);
float *mat4Rotate(float *m, float angle);
float *mat4Ortho(float *m, float left, float right, float bottom, float top);

//draw.c
void drawPolygon(const Renderer *renderer, const Polygon *polygon, float x, float y, float width, float height, float angle);
void drawPolygonMat(const Renderer *renderer, const Polygon *polygon, const float *mat);
void drawTexture(const Renderer *renderer, const Texture *tex, float x, float y, float width, float height, float angle);
void drawTextureMat(const Renderer *renderer, const Texture *tex, const float *mat);
void drawSpritesheet(const Renderer *renderer, const Spritesheet *sheet, float x, float y, float width, float height, float angle, float row, float col, byte flipped);
void drawSpritesheetMat(const Renderer *renderer, const Spritesheet *sheet, const float *mat, float row, float col, byte flipped);
Texture *drawFontTex(const Renderer *renderer, Texture *surface, const Font *font, const char *string, float r, float g, float b);
void drawDialogue(const Renderer *renderer, const Dialogue *dialogue, float x, float y, float width, float height, float angle);
void drawDialogueMat(const Renderer *renderer, const Dialogue *dialogue, const float *mat);
void drawDialogueTex(const Renderer *renderer, const Font *font, Texture *tex, const char **strings, int lines, float r, float g, float b);

//load.c
Texture *createTexture(Texture *texture, const char *path, const GLint *parameters);
Spritesheet *createSpritesheet(const Renderer *renderer, Spritesheet *spritesheet, int rows, int cols);
GLuint createShaderProgram(const char *vs_source, const char *fs_source);
Font *createFont(const Renderer *renderer, Font *font, const char *path, unsigned int height);
Polygon *createPolygon(Polygon *polygon, float *vertices, int count, GLenum primitive, float r, float g, float b, float a);

//update.c
int dialogueNext(Dialogue *dialogue);
int dialogueNextSkip(Dialogue *dialogue, char *skip, int skiplen);

//cleanup.c
void cleanupInit(void);
void cleanupDestroy(void);
void cleanupAddVao(GLuint *data, int length);
void cleanupAddVbo(GLuint *data, int length);
void cleanupAddFbo(GLuint *data, int length);
void cleanupAddTex(GLuint *data, int length);
void cleanupAddProgram(GLuint *data, int length);
void cleanupAddWindow(GLFWwindow **data, int length);
void cleanupAddAllocated(void **data, int length);
void cleanupAddAllocated_p(void ***data, int length);

#endif
