#version 450 core

uniform ivec2 mousePos; 
uniform ivec2 lastMousePos;

in vec2 pixel;
in vec2 uv;

out float value;

float sdLine(in vec2 p, in vec2 a, in vec2 b) {
	vec2 pa = p-a, ba = b-a;
	float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
	vec2 v = pa - ba*h;
	return length(v);
}

void main(){

	float d = sdLine(pixel, lastMousePos, mousePos);
	float bsize = 20;
	
	value = 1 - pow(max(0, 2 * (d / bsize)), 5);
}