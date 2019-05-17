#version 450 core

in vec3 point;

void main() {
	gl_Position.xy = 2 * point.xy - 1;
	// Pressure
	gl_Position.z = point.z;
	gl_Position.w = 1.0f;
}