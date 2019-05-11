#version 450 core

in vec2 vtx_pos;

void main() {
    gl_Position.xy = 2 * vtx_pos - 1;
    gl_Position.z = 0.0f;
    gl_Position.w = 1.0f;
}