#version 450 core

uniform sampler2D stroke;
uniform sampler2D canvas;

in vec2 uv;
out vec3 color;

void main(){
    vec3 brush_color = vec3(0);
    float stroke_alpha = texture(stroke, uv).r;
    vec3 dst = texture(canvas, uv).rgb;

    color = (1 - stroke_alpha)*dst + stroke_alpha*brush_color;
}