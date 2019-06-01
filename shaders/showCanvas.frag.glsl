#version 450 core

uniform sampler2D stroke;
uniform sampler2DArray canvas;
uniform int currentFrameLayerIndex;

in vec2 uv;
out vec3 color;

void main(){
    vec3 brush_color = vec3(0);
    float stroke_alpha = texture(stroke, uv).r;
    vec3 dst = texture(canvas, vec3(uv, currentFrameLayerIndex)).rgb;

    color = (1 - stroke_alpha)*dst + stroke_alpha*brush_color;
}