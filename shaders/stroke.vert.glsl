#version 450 core

in vec4 point;

void main() {
	gl_Position = point;
}