#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "misc.h"

#include "game001.h"

char *program_name;

void print_glfw_error(int error_code, const char *description);

Window *createWindow(Window *win, const char *title, int width, int height)
{
	win->title = (char *)xmalloc(strlen(title));
	cleanupAddAllocated((void **)&win->title, 1);
	strcpy(win->title, title);
	win->width = width;
	win->height = height;

	GLenum glewerr;

	glfwSetErrorCallback(print_glfw_error);

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	win->obj = glfwCreateWindow(width, height, title, NULL, NULL);
	cleanupAddWindow(&win->obj, 1);

	glfwMakeContextCurrent(win->obj);
	glfwSwapInterval(1);

	glewerr = glewInit();
	if(glewerr != GLEW_OK) {
		fprintf(stderr, "%s: glew failed to initialize. %s", 
				program_name, glewGetErrorString(glewerr));
	}

	return win;
}

void print_glfw_error(int error_code, const char *description)
{
	(void) error_code;
	printf("GLFW error: %s\n", description);
}
