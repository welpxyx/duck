#include <string.h>
#include <stdio.h>

#include "game001.h"

int dialogueNext(Dialogue *dialogue)
{
	Character *ch;
	char c;
	GLfloat vertices[8];

	c = dialogue->text[dialogue->pos[1]][dialogue->pos[0]];
	ch = dialogue->font->chars + (int)c;
	glBindBuffer(GL_ARRAY_BUFFER, dialogue->vbo);


	//test if we're  at the end of the line
	if(c == '\0') {
		if(dialogue->pos[1] + 1 >= dialogue->lines) {
			return -1;
		} else {
			dialogue->pos[0] = 0;
			dialogue->size[0] = ch->advance;
			dialogue->pos[1]++;
			dialogue->size[1] += dialogue->font->spacing;
			
			//every row
			vertices[4] = 0;
			vertices[5] = (float)(dialogue->size[1] + dialogue->font->spacing) / dialogue->tex.height;
			glBufferSubData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), 2 * sizeof(GLfloat), vertices + 4);

			vertices[6] = 1;
			vertices[7] = (float)dialogue->size[1] / dialogue->tex.height;
			glBufferSubData(GL_ARRAY_BUFFER, 10 * sizeof(GLfloat), 2 * sizeof(GLfloat), vertices + 6);


		}
	} else {
		dialogue->pos[0]++;
		dialogue->size[0] += ch->advance;
	}

	//every time
	vertices[0] = (float)dialogue->size[0] / dialogue->tex.width;
	vertices[1] = (float)dialogue->size[1] / dialogue->tex.height;
	vertices[2] = (float)dialogue->size[0] / dialogue->tex.width;
	vertices[3] = (float)(dialogue->size[1] + dialogue->font->spacing) / dialogue->tex.height;

	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(GLfloat), vertices);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return c;
}

int dialogueNextSkip(Dialogue *dialogue, char *skip, int skiplen)
{
	char c;
	int i, j;

	if(!skiplen && skip)
		skiplen = strlen(skip);

	for(j = 0; (c = dialogueNext(dialogue)) != -1; j++) {
		for(i = 0; i < skiplen; i++)
			if(c == skip[i])
				break;

		if(!skip)
			continue;

		if(c != skip[i])
			return j;
	}

	return j;
}
