#version 450 core

uniform sampler2D stroke;

in vec2 uv;
out vec3 color;

void main(){
    color = vec3(texture(stroke, uv).r);
    color = 1 - color;
}