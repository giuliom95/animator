#version 450 core

uniform sampler2D canvas;
uniform ivec2 mousePos; 

in vec2 pixel;

out vec3 color;

void main(){

    vec3 old = texelFetch(canvas, ivec2(pixel), 0).rgb;

    vec2 diff = pixel - mousePos;
    float sqdist = dot(diff, diff);

    float brush_size = 2;
    float a = exp(-sqdist/(brush_size*brush_size));

    color = a*vec3(1,0,0) + (1-a)*old;
}