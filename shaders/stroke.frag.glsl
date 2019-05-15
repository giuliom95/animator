#version 450 core

in vec2 uv;

out float value;

void main(){
    value = 1 - pow(max(0, 2 * length(2*uv - 1) - 1), 4);
}