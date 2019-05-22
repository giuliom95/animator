#version 450 core

in vec3 point;

void main() {
	// z contains brush pressure
	gl_Position.xyz = point;
	gl_Position.w = 1.0f;
}