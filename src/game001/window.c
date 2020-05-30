#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "misc.h"

#include "game001.h"

char *program_name;

int print_glfw_error(void);

Window *createWindow(Window *win, const char *title, int width, int height)
{
	win->title = (char *)xmalloc(strlen(title));
	cleanupAddAllocated((void **)&win->title, 1);
	strcpy(win->title, title);
	win->width = width;
	win->height = height;

	GLenum glewerr;

	if(!glfwInit())
		print_glfw_error();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwSwapInterval(1);

	win->obj = glfwCreateWindow(width, height, title, NULL, NULL);
	cleanupAddWindow(&win->obj, 1);

	if(win->obj == NULL)
		print_glfw_error();

	glfwMakeContextCurrent(win->obj);

	glewerr = glewInit();
	if(glewerr != GLEW_OK) {
		fprintf(stderr, "%s: glew failed to initialize. %s", 
				program_name, glewGetErrorString(glewerr));
	}

	return win;
}

int print_glfw_error(void)
{
	char errormsg[128];
	int code = glfwGetError((const char **)errormsg);
	
	printf("GLFW failed to initialize: %s\n", errormsg);

	return code;
}
