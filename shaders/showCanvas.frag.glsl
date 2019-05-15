#version 450 core

uniform sampler2D stroke;
uniform sampler2D canvas;

in vec2 uv;
out vec3 color;

void main(){
    float s = 1 - texture(stroke, uv).r;
    //vec3 c = texture(canvas, uv).rgb;

    color = vec3(s);
}