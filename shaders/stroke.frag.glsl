#version 450 core

uniform sampler2D stroke;

in vec2 local_uv;
in vec2 global_uv;

out float color;

void main(){
    float v = 1 - pow(max(0, 2 * length(2*local_uv - 1) - 1), 4);
    float under = texture(stroke, global_uv).r;
    color = max(under, v);
}