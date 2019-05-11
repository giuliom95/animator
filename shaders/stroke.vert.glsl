#version 450 core

in vec2 vtx_pos;

void main() {
    gl_Position.xy = vtx_pos;
    gl_Position.z = 0.0f;
    gl_Position.w = 1.0f;
}