#include <stdio.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "graphics.h"
#include "misc.h"

#include "game001.h"

#define SHADER_BUFSIZ 512
char *program_name;

GLuint compile_shader(const char *path, char *buf, GLenum shader_type);


Texture *createTexture(Texture *texture, const char *path, const GLint *parameters)
{
	int nrChannels;
	unsigned char *data;

	glGenTextures(1, &texture->id);
	cleanupAddTex(&texture->id, 1);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	if(parameters) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameters[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameters[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameters[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameters[3]);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	data = stbi_load(path, &texture->width, &texture->height, &nrChannels, 0);
	if(data) {
		int glChannels = (nrChannels==3)?GL_RGB:GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, glChannels, texture->width,
				texture->height, 0, glChannels,
				GL_UNSIGNED_BYTE, data);
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

Spritesheet *createSpritesheet(const Renderer *renderer,
		Spritesheet *spritesheet, int rows, int cols)
{
	Texture *tex = &spritesheet->tex;
	GLuint obj[4];

	float vertices[] = {
		 1.0f/cols, 0.0f,
		 1.0f/cols, 1.0f/rows,
		 0.0f,      1.0f/rows,
		 0.0f,      0.0f
	};
	
	float vertices_flipped[] = {
		 0.0f,      0.0f,
		 0.0f,      1.0f/rows,
		 1.0f/cols, 1.0f/rows,
		 1.0f/cols, 0.0f
	};


	spritesheet->cols = cols;
	spritesheet->rows = rows;
	spritesheet->fwidth = tex->width/cols;
	spritesheet->fheight = tex->height/rows;

	glGenBuffers(2, obj + 2);
	cleanupAddVbo(obj + 2, 2);
	glGenVertexArrays(2, obj);
	cleanupAddVao(obj, 2);
	spritesheet->vao = obj[0];
	spritesheet->flippedvao = obj[1];

	glBindVertexArray(spritesheet->vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, renderer->quadvbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, obj[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->quadebo);
	

	glBindVertexArray(spritesheet->flippedvao);
	
	glBindBuffer(GL_ARRAY_BUFFER, renderer->quadvbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, obj[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_flipped), vertices_flipped, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->quadebo);

	
	return spritesheet;
}

GLuint createShaderProgram(const char *vs_source, const char *fs_source)
{
	int success;
	GLuint vs_id, fs_id, id;
	char buf[SHADER_BUFSIZ];

	vs_id = compile_shader(vs_source, buf, GL_VERTEX_SHADER);
	fs_id = compile_shader(fs_source, buf, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	cleanupAddProgram(&id, 1);
	glAttachShader(id, vs_id);
	glAttachShader(id, fs_id);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(id, SHADER_BUFSIZ, NULL, buf);
		fprintf(stderr, "%s: failed to compile shader program: %s\n", program_name, buf);
		return 0;
	}

	glDeleteShader(vs_id);
	glDeleteShader(fs_id);
	
	return id;
}

GLuint compile_shader(const char *path, char *buf, GLenum shader_type)
{
	int c, i, result;
	GLuint id;
	FILE *file;

	file = fopen(path, "r");
	if(file == NULL)
		perror(program_name);

	for(i = 0; (c = getc(file)) != EOF; i++)
		buf[i] = c;

	fclose(file);
	
	id = glCreateShader(shader_type);
	glShaderSource(id, 1, (const char * const *)&buf, &i);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(!result) {
		glGetShaderInfoLog(id, SHADER_BUFSIZ, NULL, buf);
		fprintf(stderr, "%s: failed to compile shader %s: %s\n", program_name, path, buf);
		return 0;
	}

	return id;
}

Font *createFont(const Renderer *renderer, Font *font,
		const char *path, unsigned int height)
{
	FT_Library ft;
	FT_Face face;
	FT_Error error;
	int c;
	unsigned int adv_px;
	float xpos;

	if((error = FT_Init_FreeType(&ft)))
		fprintf(stderr, "%s: failed to initialize FreeType: %s\n",
				program_name, FT_Error_String(error));
	
	if(FT_New_Face(ft, path, 0, &face))
		fprintf(stderr, "%s: failed to load font %s: %s\n",
				program_name, path, FT_Error_String(error));

	FT_Set_Pixel_Sizes(face, 0, height);
	font->height = height;
	font->spacing = face->size->metrics.height >> 6;
	adv_px = (face->size->metrics.max_advance >> 6) + 1;

	font->tex.width = adv_px * 128;
	font->tex.height = font->spacing;
	font->fwidth = adv_px;
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &font->tex.id);
	cleanupAddTex(&font->tex.id, 1);
	glBindTexture(GL_TEXTURE_2D, font->tex.id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->tex.width,
			font->tex.height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

	for(xpos = 0, c = 0; c < 128; c++) {
		if((error = FT_Load_Char(face, c, FT_LOAD_RENDER))) {
			fprintf(stderr, "%s: failed to load gliph %c: %s\n",
					program_name, c,
					FT_Error_String(error));
			continue;
		}

		font->chars[c].size[0] = face->glyph->bitmap.width;
		font->chars[c].size[1] = face->glyph->bitmap.rows;
		font->chars[c].bearing[0] = face->glyph->bitmap_left;
		font->chars[c].bearing[1] = face->glyph->bitmap_top;
		font->chars[c].advance = face->glyph->advance.x >> 6;
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, xpos, 0,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows, GL_RED,
				GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		xpos += adv_px;
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return font;
}

Polygon *createPolygon(Polygon *polygon, float *vertices, int count, GLenum primitive, float r, float g, float b, float a)
{
	int i;
	float width, height;
	float *new_vertices;

	width = height = 0;

	new_vertices = (float *)xmalloc(2 * count * sizeof(float));

	for(i = 0; i < count * 2; i += 2) {
		if(vertices[i] > width)
			width = vertices[i];
		if(vertices[i+1] > height)
			height = vertices[i+1];
	}

	for(i = 0; i < count * 2; i += 2) {
		new_vertices[i] = vertices[i] / width;
		new_vertices[i+1] = vertices[i+1] / height;
	}
	
	glGenBuffers(1, &polygon->vbo);
	cleanupAddVbo(&polygon->vbo, 1);
	glGenVertexArrays(1, &polygon->vao);
	cleanupAddVao(&polygon->vao, 1);

	glBindVertexArray(polygon->vao);
	glBindBuffer(GL_ARRAY_BUFFER, polygon->vbo);
	glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), new_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	free(new_vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	polygon->color[0] = r;
	polygon->color[1] = g;
	polygon->color[2] = b;
	polygon->color[3] = a;
	polygon->size[0] = width;
	polygon->size[1] = height;
	polygon->indices = count;
	polygon->primitive = primitive;

	return polygon;
}
