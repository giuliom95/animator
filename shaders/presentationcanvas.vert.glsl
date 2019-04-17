#version 450 core

uniform mat4 view;

layout(location = 0) in vec2 vtx_pos;
layout(location = 1) in vec2 vtx_uv;

out vec2 uv;

void main() {
    gl_Position.xy = vtx_pos;
    gl_Position.z = 0.0f;
    gl_Position.w = 1.0f;
    gl_Position = gl_Position * view;
    uv = vtx_uv;
}