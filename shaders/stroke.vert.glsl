#version 450 core

layout(location = 0) in vec2 vtx_pos;
layout(location = 1) in vec2 vtx_uv;

out vec2 pixel;
out vec2 uv;

void main() {
    gl_Position.xy = 2*(vtx_uv - .5);
    gl_Position.z = 0.0f;
    gl_Position.w = 1.0f;
    pixel = vtx_pos;
    uv = gl_Position.xy;
}