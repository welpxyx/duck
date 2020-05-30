#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"

#include "game001.h"
#include "graphics.h"

char *program_name;

void setup_locations(int len, GLuint program, GLint **locations, char **names);

void rendererInit(Renderer *renderer)
{
	GLuint obj[4];

	renderer->shadercount = SHADER_COUNT;
	
	renderer->shaders[SHADERS_TEXTURE] = createShaderProgram("shaders/texture.vert", "shaders/texture.frag");
	renderer->shaders[SHADERS_TEXT] = createShaderProgram("shaders/text.vert", "shaders/text.frag");
	renderer->shaders[SHADERS_POLYGON] = createShaderProgram("shaders/polygon.vert", "shaders/polygon.frag");

	/*for(i = 0; i < count; i++)
		renderer->shaders[i] = shaders[i];*/
	
	GLfloat vertices[] = {
		 1.0f,  0.0f,
		 1.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  0.0f
	};


	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	char *names[4] = {
		"model",
		"projection",
		"view",
		"texOffset"
	};
	
	//set view and projection arrays
	mat4Ortho(renderer->proj, 0.0f, renderer->window.width, renderer->window.height, 0.0f);
	mat4Identity(renderer->view);

	//get uniform locations for shader programs
	setup_locations(4, renderer->shaders[SHADERS_TEXTURE], renderer->uniforms, names);
	names[3] = "color";
	setup_locations(4, renderer->shaders[SHADERS_POLYGON], renderer->uniforms + 2, names);
	names[0] = "textColor";
	setup_locations(2, renderer->shaders[SHADERS_TEXT], renderer->uniforms + 1, names);

	//set view and projection uniforms for shader programs
	glUseProgram(renderer->shaders[SHADERS_TEXTURE]);
	glUniformMatrix4fv(renderer->uniforms[SHADERS_TEXTURE][UNIFORMS_VIEW], 1, GL_FALSE, renderer->view);
	glUniformMatrix4fv(renderer->uniforms[SHADERS_TEXTURE][UNIFORMS_PROJECTION], 1, GL_FALSE, renderer->proj);
	glUseProgram(renderer->shaders[SHADERS_POLYGON]);
	glUniformMatrix4fv(renderer->uniforms[SHADERS_POLYGON][UNIFORMS_VIEW], 1, GL_FALSE, renderer->view);
	glUniformMatrix4fv(renderer->uniforms[SHADERS_POLYGON][UNIFORMS_PROJECTION], 1, GL_FALSE, renderer->proj);

	glGenVertexArrays(2, obj);
	cleanupAddVao(obj, 2);
	glGenBuffers(2, obj + 2);
	cleanupAddVbo(obj + 2, 2);

	//set up objects for textures
	renderer->quadvao = obj[0];
	renderer->quadvbo = obj[2];
	glBindVertexArray(obj[0]);
	glBindBuffer(GL_ARRAY_BUFFER, obj[2]);
	glGenBuffers(1, &renderer->quadebo);
	cleanupAddVbo(&renderer->quadebo, 1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->quadebo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);


	//set up objects for text rendering
	renderer->textvao = obj[1];
	renderer->textvbo = obj[3];
	glBindVertexArray(obj[1]);
	glBindBuffer(GL_ARRAY_BUFFER, obj[3]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->quadebo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	glGenFramebuffers(1, &renderer->textfb);
	cleanupAddFbo(&renderer->textfb, 1);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void setup_locations(int len, GLuint program, GLint **locations, char **names)
{
	*locations = (GLint *)xmalloc(len * sizeof(GLint));
	cleanupAddAllocated((void **)locations, 1);

	while(len > 0) {
		len--;
		(*locations)[len] = glGetUniformLocation(program, names[len]);
	}
}

//make sure to unbind everything
void setupTextFB(const Renderer *renderer, GLuint shader, GLint projLoc, GLuint *tcb, GLsizei width, GLsizei height, int *oldv, GLint fmt)
{
	float proj[16];

	glUseProgram(shader);

	glGenTextures(1, tcb);
	cleanupAddTex(tcb, 1);
	glBindTexture(GL_TEXTURE_2D, *tcb);

	glBindFramebuffer(GL_FRAMEBUFFER, renderer->textfb);

	glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt,
			GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tcb, 0);

	glGetIntegerv(GL_VIEWPORT, oldv);
	glViewport(0, 0, width, height);

	mat4Ortho(proj, 0.0f, width, 0.0f, height);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj);

	glBindVertexArray(renderer->textvao);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->textvbo);
}

Dialogue *setupDialogue(const Renderer *renderer, Font
		*font, Dialogue *dialogue, int lines, const char **text,
		int r, int g, int b)
{
	int i;
	float centerheight;
	
	dialogue->lines = lines;
	dialogue->text = (char **)xmalloc(lines * sizeof(char *));
	cleanupAddAllocated_p((void ***)&dialogue->text, 1);
	dialogue->font = font;
	dialogue->pos[0] = dialogue->pos[1] = dialogue->size[0] = dialogue->size[1] = 0;

	for(i = 0; i < lines; i++) {
		dialogue->text[i] = (char *)xmalloc(strlen(text[i]));
		cleanupAddAllocated((void **)dialogue->text + i, 1);
		strcpy(dialogue->text[i], text[i]);
	}

	drawDialogueTex(renderer, font, &dialogue->tex, (const char **)text, lines, r, g, b);
	
	centerheight = (float)(dialogue->font->spacing) / dialogue->tex.height;

	GLfloat vertices[] = {
		0, 0,
		0, centerheight,
		0, centerheight,
		0, 0,
		1, 0,
		1, 0
	};
	
	glGenVertexArrays(1, &dialogue->vao);
	cleanupAddVao(&dialogue->vao, 1);
	glGenBuffers(1, &dialogue->vbo);
	cleanupAddVbo(&dialogue->vbo, 1);
	glBindVertexArray(dialogue->vao);

	glBindBuffer(GL_ARRAY_BUFFER, dialogue->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);
	return dialogue;
}
