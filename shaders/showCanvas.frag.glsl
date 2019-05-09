#version 450 core

uniform sampler2D canvas;

in vec2 uv;
out vec4 color;

void main(){
    vec3 img = texture(canvas, uv).rgb;

    color = vec4(img, .1);
}