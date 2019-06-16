#version 450 core

uniform sampler2D stroke;
uniform sampler2DArray canvas;
uniform int currentFrameLayerIndex;

in vec2 uv;
out vec4 color;

void main(){
    vec3 brush_color = vec3(0);
    float stroke_alpha = texture(stroke, uv).r;
    vec4 old = texture(canvas, vec3(uv, currentFrameLayerIndex));

    color.rgb = (1 - stroke_alpha)*old.rgb + stroke_alpha*brush_color;
    color.a = (1 - stroke_alpha)*old.a + stroke_alpha;
}