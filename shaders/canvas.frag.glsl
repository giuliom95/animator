#version 450 core

in vec2 vtx_pos;

out vec3 color;

void main(){

    // vec2 diff = vec2(.5) - uv;
    // float sqdist = dot(diff, diff);

    // float brush_size = .1;
    // float a = exp(-sqdist/(brush_size*brush_size));

    color = vec3(1, 0, 0);
}