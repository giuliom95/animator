#version 450 core

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

out vec2 local_uv;
out vec2 global_uv;

void main() {
	vec2 v0 = gl_in[0].gl_Position.xy;
	vec2 v1 = gl_in[1].gl_Position.xy;
	vec2 d = normalize(v1 - v0);
	vec2 td = vec2(d.y, -d.x);

	float s = .03;

	gl_Position = vec4(v0 + s*td - s*d,	0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(0.0, 0.0); EmitVertex();
	gl_Position = vec4(v0 - s*td - s*d,	0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(0.0, 1.0); EmitVertex();
	gl_Position = vec4(v0 + s*td,		0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(0.5, 0.0); EmitVertex();
	gl_Position = vec4(v0 - s*td,		0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(0.5, 1.0); EmitVertex();
	gl_Position = vec4(v1 + s*td,		0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(0.5, 0.0); EmitVertex();
	gl_Position = vec4(v1 - s*td,		0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(0.5, 1.0); EmitVertex();
	gl_Position = vec4(v1 + s*td + s*d,	0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(1.0, 0.0); EmitVertex();
	gl_Position = vec4(v1 - s*td + s*d,	0, 1); global_uv = .5 * gl_Position.st + .5; local_uv = vec2(1.0, 1.0); EmitVertex();

	EndPrimitive();
}