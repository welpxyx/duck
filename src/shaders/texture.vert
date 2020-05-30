#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

out vec2 texCoordsFinal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec2 texOffset;

void main() {
	gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
	texCoordsFinal = texCoords + texOffset;
}
