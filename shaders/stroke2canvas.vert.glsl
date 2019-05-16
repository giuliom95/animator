#version 450 core

in vec2 vtx_uv;

out vec2 uv;

void main() {
    gl_Position.xy = 2*(vtx_uv - .5);
    gl_Position.z = 0.0f;
    gl_Position.w = 1.0f;
    uv = vtx_uv;
}