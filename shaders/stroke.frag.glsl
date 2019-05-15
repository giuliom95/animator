#version 450 core

in vec2 uv;

out float stroke;

void main(){
    float v = 1 - pow(max(0, 2 * length(2*uv - 1) - 1), 4);
	stroke = v;
}