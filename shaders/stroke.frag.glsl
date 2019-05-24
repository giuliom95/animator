#version 450 core

in vec3 uvb;

out float stroke;

void main(){
	float bs = 1.5;				// Blur Size
	float isbs = uvb.z / bs;	// Inverted Scaled Blur Size

	//float v = 1 - pow(max(0, 2 * length(2*uvb.st - 1) - 1), 4);
	vec2 uv = 2*uvb.st - 1;
	float v = min(1, -isbs*(length(uv) - 1));
	stroke = v;
}