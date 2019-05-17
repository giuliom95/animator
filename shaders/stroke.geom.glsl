#version 450 core

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

out vec2 uv;

void main() {
	vec2 v0 = gl_in[0].gl_Position.xy;
	vec2 v1 = gl_in[1].gl_Position.xy;
	vec2 d = v1 - v0;
	vec2 dn = dot(d, d) < 0.00001 ? vec2(1,0) : normalize(d);
	vec2 td = vec2(dn.y, -dn.x);

	float s = .02;
	float s0 = s * gl_in[0].gl_Position.z;
	float s1 = s * gl_in[1].gl_Position.z;

	gl_Position = vec4(v0 + s0*td - s0*dn,	0, 1); uv = vec2(0.0, 0.0); EmitVertex();
	gl_Position = vec4(v0 - s0*td - s0*dn,	0, 1); uv = vec2(0.0, 1.0); EmitVertex();
	gl_Position = vec4(v0 + s0*td,		    0, 1); uv = vec2(0.5, 0.0); EmitVertex();
	gl_Position = vec4(v0 - s0*td,		    0, 1); uv = vec2(0.5, 1.0); EmitVertex();
	gl_Position = vec4(v1 + s1*td,		    0, 1); uv = vec2(0.5, 0.0); EmitVertex();
	gl_Position = vec4(v1 - s1*td,		    0, 1); uv = vec2(0.5, 1.0); EmitVertex();
	gl_Position = vec4(v1 + s1*td + s1*dn,	0, 1); uv = vec2(1.0, 0.0); EmitVertex();
	gl_Position = vec4(v1 - s1*td + s1*dn,	0, 1); uv = vec2(1.0, 1.0); EmitVertex();

	EndPrimitive();
}