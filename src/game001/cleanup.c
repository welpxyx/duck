#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "misc.h"

#include "game001.h"

#define INDEX_VAO 0
#define INDEX_VBO 1
#define INDEX_FBO 2
#define INDEX_TEX 3
#define INDEX_PROGRAM 4
#define INDEX_WINDOW 5
#define INDEX_ALLOCATED 6
#define INDEX_ALLOCATED_P 7

#define CLEANUP_ADD(data, length, index, list, type) \
	int i, oldlen;\
\
	oldlen = cleanup.sizes[(index)];\
	cleanup.sizes[(index)] += length;\
	list = (type *)xrealloc(list, cleanup.sizes[index] * sizeof(type));\
\
	for(i = 0; i < length; i++)\
		(list)[i + oldlen] = data[i];

extern Cleanup cleanup;

void cleanupInit(void)
{
	int i;

	cleanup.vao = NULL;
	cleanup.vbo = NULL;
	cleanup.fbo = NULL;
	cleanup.tex = NULL;
	cleanup.program = NULL;
	cleanup.window = NULL;
	cleanup.allocated = NULL;
	cleanup.allocated_p = NULL;

	for(i = 0; i < 8; i++) 
		cleanup.sizes[i] = 0;
}

void cleanupDestroy(void)
{
	int i;

	glDeleteVertexArrays(cleanup.sizes[INDEX_VAO], cleanup.vao);
	glDeleteBuffers(cleanup.sizes[INDEX_VBO], cleanup.vbo);
	glDeleteFramebuffers(cleanup.sizes[INDEX_FBO], cleanup.fbo);
	glDeleteTextures(cleanup.sizes[INDEX_TEX], cleanup.tex);
	for(i = 0; i < cleanup.sizes[INDEX_PROGRAM]; i++)
		glDeleteProgram(cleanup.program[i]);
	for(i = 0; i < cleanup.sizes[INDEX_ALLOCATED]; i++)
		free(cleanup.allocated[i]);
	for(i = 0; i < cleanup.sizes[INDEX_ALLOCATED_P]; i++)
		free(cleanup.allocated_p[i]);

	free(cleanup.vao);
	free(cleanup.vbo);
	free(cleanup.fbo);
	free(cleanup.tex);
	free(cleanup.program);
	free(cleanup.window);
	free(cleanup.allocated);
	free(cleanup.allocated_p);
}

void cleanupAddVao(GLuint *data, int length)
{
	CLEANUP_ADD(data, length, INDEX_VAO, cleanup.vao, GLuint);
}

void cleanupAddVbo(GLuint *data, int length)
{
	CLEANUP_ADD(data, length, INDEX_VBO, cleanup.vbo, GLuint);
}

void cleanupAddFbo(GLuint *data, int length)
{
	CLEANUP_ADD(data, length, INDEX_FBO, cleanup.fbo, GLuint);
}

void cleanupAddTex(GLuint *data, int length)
{
	CLEANUP_ADD(data, length, INDEX_TEX, cleanup.tex, GLuint);
}

void cleanupAddProgram(GLuint *data, int length)
{
	CLEANUP_ADD(data, length, INDEX_PROGRAM, cleanup.program, GLuint);
}

void cleanupAddWindow(GLFWwindow **data, int length)
{
	CLEANUP_ADD(data, length, INDEX_WINDOW, cleanup.window, GLFWwindow *);
}

void cleanupAddAllocated(void **data, int length)
{
	CLEANUP_ADD(data, length, INDEX_ALLOCATED, cleanup.allocated, void *);
}

void cleanupAddAllocated_p(void ***data, int length)
{
	CLEANUP_ADD(data, length, INDEX_ALLOCATED_P, cleanup.allocated_p, void **);
}
