#version 450 core

uniform sampler2D image;

in vec2 uv;
out vec3 color;

void main(){
    vec3 img = texture(image, uv).rgb;
    color = vec3(uv, 0);
}