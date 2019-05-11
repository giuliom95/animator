#version 450 core

layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

void main() {
    vec2 v0 = gl_in[0].gl_Position.xy;
    vec2 v1 = gl_in[1].gl_Position.xy;
    vec2 d = normalize(v1 - v0);
    vec2 td = vec2(d.y, -d.x);

    gl_Position = vec4(v0 - .01* d, 0, 1); EmitVertex();
    gl_Position = vec4(v0 + .01*td, 0, 1); EmitVertex();
    gl_Position = vec4(v0 - .01*td, 0, 1); EmitVertex();
    gl_Position = vec4(v1 + .01*td, 0, 1); EmitVertex();
    gl_Position = vec4(v1 - .01*td, 0, 1); EmitVertex();
    gl_Position = vec4(v1 + .01* d, 0, 1); EmitVertex();

    EndPrimitive();
}