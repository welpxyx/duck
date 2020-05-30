#include "game001.h"
#include <GL/glew.h>

#define SHADERS_TEXTURE 0
#define SHADERS_TEXT    1
#define SHADERS_POLYGON 2

#define SHADER_COUNT 3

#define UNIFORMS_MODEL      0
#define UNIFORMS_TEXTCOLOR  0
#define UNIFORMS_PROJECTION 1
#define UNIFORMS_VIEW       2
#define UNIFORMS_OFFSET     3
#define UNIFORMS_COLOR      3

void setupTextFB(const Renderer *renderer, GLuint shader, GLint projLoc, GLuint *tcb, GLsizei width, GLsizei height, int *oldv, GLint fmt);
