#version 330 core

in vec2 texCoordsFinal;
out vec4 FragColor;

uniform sampler2D tex;

void main() {
	FragColor = vec4(1, 0, 0, 1);
}
