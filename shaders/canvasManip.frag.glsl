#version 450 core

uniform sampler2D canvas;
uniform ivec2 mousePos; 
uniform ivec2 lastMousePos;

in vec2 pixel;

out vec3 color;

float sqSdLine(in vec2 p, in vec2 a, in vec2 b) {
    vec2 pa = p-a, ba = b-a;
    float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
    vec2 v = pa - ba*h;
    return dot(v, v);
}

void main(){

    vec3 old = texelFetch(canvas, ivec2(pixel), 0).rgb;

    float sqdist = sqSdLine(pixel, lastMousePos, mousePos);

    float bsize = 2;
    
    float a = step(-bsize*bsize, -sqdist);

    color = a*vec3(0,0,0) + (1-a)*old;
}