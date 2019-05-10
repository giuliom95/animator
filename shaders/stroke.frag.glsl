#version 450 core

const int STROKE_SIZE = 1024;

uniform sampler2D stroke;
uniform ivec2 mousePos; 
uniform ivec2 lastMousePos;

layout (std140) uniform strokeBlock {
    uint strokePoints[2*STROKE_SIZE];
};


in vec2 pixel;

out float color;

float sqSdLine(in vec2 p, in vec2 a, in vec2 b) {
    vec2 pa = p-a, ba = b-a;
    float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
    vec2 v = pa - ba*h;
    return dot(v, v);
}

void main(){

    float old = texelFetch(stroke, ivec2(pixel), 0).r;

    float sqdist = sqrt(sqSdLine(pixel, lastMousePos, mousePos));
    
    //float a = exp(-ibsize*pow(sqdist, 3));
    float a = max(1-sqdist, 0);

    color = min(old + a, 1);
    color = strokePoints[0] / 255.0;

    //color = strokePoints[0] == 4294967295 ? 1 : 0;
    color = strokePoints[0] < 4294967000 ? 1 : 0;
    // 0 -> white; 1 -> black
}