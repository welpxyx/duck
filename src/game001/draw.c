#include <GL/glew.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "graphics.h"

#include "game001.h"

#define SPRITE_FRAMEX(spritesheet, col) (((spritesheet).fwidth * (col)) / (spritesheet).tex.width)
#define SPRITE_FRAMEY(spritesheet, row) (((spritesheet).fheight * (row)) / (spritesheet).tex.height)

void setModel(float model[16], float x, float y, float width, float height, float angle);
void gen_text_rects(const Font *font, const char *string, GLfloat
		*rects, int *size, int length, int height);

void drawPolygon(const Renderer *renderer, const Polygon *polygon, float x, float y, float width, float height, float angle)
{
	float model[16];

	setModel(model, x, y, width, height, angle);
	drawPolygonMat(renderer, polygon, model);
}

void drawPolygonMat(const Renderer *renderer, const Polygon *polygon, const float *mat)
{
	GLint *uniforms = renderer->uniforms[SHADERS_POLYGON];

	glUseProgram(renderer->shaders[SHADERS_POLYGON]);
	glUniformMatrix4fv(uniforms[UNIFORMS_MODEL], 1, GL_FALSE, mat);
	glUniform4fv(uniforms[UNIFORMS_COLOR], 1, polygon->color);

	glBindVertexArray(polygon->vao);
	glDrawArrays(polygon->primitive, 0, polygon->indices);
}

void drawTextureMat(const Renderer *renderer, const Texture *tex, const float *mat)
{
	GLint *uniforms = renderer->uniforms[SHADERS_TEXTURE];

	glUseProgram(renderer->shaders[SHADERS_TEXTURE]);
	glUniformMatrix4fv(uniforms[UNIFORMS_MODEL], 1, GL_FALSE, mat);
	glUniform2f(uniforms[UNIFORMS_COLOR], 0.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, tex->id);
	glBindVertexArray(renderer->quadvao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawTexture(const Renderer *renderer, const Texture *tex, float x, float y, float width, float height, float angle)
{
	float model[16];

	setModel(model, x, y, width, height, angle);
	drawTextureMat(renderer, tex, model);
}

void drawSpritesheetMat(const Renderer *renderer, const Spritesheet *sheet, const float *mat, float row, float col, byte flipped)
{
	GLint *uniforms = renderer->uniforms[SHADERS_TEXTURE];
	
	glUseProgram(renderer->shaders[SHADERS_TEXTURE]);
	glUniformMatrix4fv(uniforms[UNIFORMS_MODEL], 1, GL_FALSE, mat);
	glUniform2f(uniforms[UNIFORMS_COLOR], SPRITE_FRAMEX(*sheet, col), SPRITE_FRAMEY(*sheet, row));

	glBindTexture(GL_TEXTURE_2D, sheet->tex.id);
	glBindVertexArray((flipped?sheet->flippedvao:sheet->vao));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawSpritesheet(const Renderer *renderer, const Spritesheet *sheet, float x, float y, float width, float height, float angle, float row, float col, byte flipped)
{
	float model[16];

	setModel(model, x, y, width, height, angle);
	drawSpritesheetMat(renderer, sheet, model, row, col, flipped);
}

void drawDialogueMat(const Renderer *renderer, const Dialogue *dialogue, const float *mat)
{
	GLint *uniforms = renderer->uniforms[SHADERS_TEXTURE];

	glUseProgram(renderer->shaders[SHADERS_TEXTURE]);
	glUniformMatrix4fv(uniforms[UNIFORMS_MODEL], 1, GL_FALSE, mat);
	glUniform2f(uniforms[UNIFORMS_COLOR], 0.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, dialogue->tex.id);
	glBindVertexArray(dialogue->vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
}

void drawDialogue(const Renderer *renderer, const Dialogue *dialogue, float x, float y, float width, float height, float angle)
{
	float model[16];

	setModel(model, x, y, width, height, angle);
	drawDialogueMat(renderer, dialogue, model);
}

Texture *drawFontTex(const Renderer *renderer, Texture *surface, const Font *font, const char *string, float r, float g, float b)
{
	int length;
	int rectsize;
	int oldv[4];
	int size[3];
	float *rects;
	GLuint tcb;
	GLint *uniforms;
	
	uniforms = renderer->uniforms[SHADERS_TEXT];
	length = strlen(string);
	rectsize = 24 * length * sizeof(float);
	rects = (float *)xmalloc(rectsize);

	
	gen_text_rects(font, string, rects, size, strlen(string), 0);

	setupTextFB(renderer, renderer->shaders[SHADERS_TEXT], uniforms[UNIFORMS_PROJECTION],
			&tcb, size[0], size[1], oldv, GL_RGBA);

	glUniform3f(uniforms[UNIFORMS_TEXTCOLOR], r, g, b);
	
	glBufferData(GL_ARRAY_BUFFER, rectsize, rects, GL_STREAM_DRAW);
	glBindTexture(GL_TEXTURE_2D, font->tex.id);
	glDrawArrays(GL_TRIANGLES, 0, length * 6);

	glViewport(oldv[0], oldv[1], oldv[2], oldv[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	free(rects);
	
	surface->id = tcb;
	surface->width = size[0];
	surface->height = size[1];

	return surface;
}
	
void drawDialogueTex(const Renderer *renderer, const Font *font, Texture *tex, const char **strings, int lines, float r, float g, float b)
{
	int i, length, rectsize, texwidth, rectpos;
	int currentsize[2];
	int size[3];
	int oldv[4];
	float *rects;
	GLint *uniforms;
	
	uniforms = renderer->uniforms[SHADERS_TEXT];

	texwidth = length = rectsize = rectpos = 0;
	rects = NULL;

	for(i = 0; i < lines; i++) {
		currentsize[0] = strlen(strings[i]);
		currentsize[1] = 24 * currentsize[0];
		
		rectsize += currentsize[1];
		length += currentsize[0];

		rects = (float *)realloc(rects, rectsize * sizeof(float));

		gen_text_rects(font, strings[i], rects + rectsize - currentsize[1], size, currentsize[0], font->spacing * i);
		
		texwidth = MAX(texwidth, size[0]);
	}

	setupTextFB(renderer, renderer->shaders[SHADERS_TEXT], uniforms[UNIFORMS_PROJECTION],
			&tex->id, texwidth, font->spacing * lines, oldv, GL_RGBA);

	glBufferData(GL_ARRAY_BUFFER, rectsize * sizeof(float), rects, GL_STREAM_DRAW);
	glBindTexture(GL_TEXTURE_2D, font->tex.id);
	glDrawArrays(GL_TRIANGLES, 0, length * 6);
	glUniform3f(uniforms[UNIFORMS_TEXTCOLOR], r, g, b);

	glViewport(oldv[0], oldv[1], oldv[2], oldv[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	free(rects);

	tex->width = texwidth;
	tex->height = font->spacing * lines;
}

void gen_text_rects(const Font *font, const char *string, GLfloat
		*rects, int *size, int length, int height)
{
	int i, x, pos_x, pos_y;
	float texpos[3];
	Character *ch;

	x = pos_y = pos_x = 0;

	ch = NULL;

	for(i = 0; i < length; i++) {
		ch = ((Font *)font)->chars + string[i];
		pos_x = x + ch->bearing[0];
		pos_y = font->height - ch->bearing[1] - 1 + height;
		//position where the character is located
		texpos[0] = ((float)font->fwidth * string[i]) / font->tex.width;
		texpos[1] = ((float)font->fwidth * string[i] + ch->size[0]) / font->tex.width;
		texpos[2] = (float)ch->size[1] / font->tex.height;

		//left
		rects[i*24+8]  = rects[i*24+16] = rects[i*24+20] = pos_x;
		rects[i*24+10] = rects[i*24+18] = rects[i*24+22] = texpos[0];
		//right
		rects[i*24+0]  = rects[i*24+4]  = rects[i*24+12] = pos_x + ch->size[0];
		rects[i*24+2]  = rects[i*24+6]  = rects[i*24+14] = texpos[1];
		//up
		rects[i*24+1]  = rects[i*24+9]  = rects[i*24+21] = pos_y;
		rects[i*24+7]  = rects[i*24+15] = rects[i*24+19] = texpos[2];
		//down
		rects[i*24+5]  = rects[i*24+13] = rects[i*24+17] = pos_y + ch->size[1];
		rects[i*24+3]  = rects[i*24+11] = rects[i*24+23] = 0;
		
		x += ch->advance;
	}

	x += ch->advance;
	
	size[0] = x;
	size[1] = font->spacing;
}

void setModel(float model[16], float x, float y, float width, float height, float angle)
{
	mat4Identity(model);
	mat4Translate(model, x, y);
	mat4Translate(model, width/2, height/2);
	mat4Rotate(model, RADIANS(angle));
	mat4Translate(model, -width/2, -height/2);
	mat4Scale(model, width, height);
}
