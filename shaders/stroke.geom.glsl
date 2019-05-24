#version 450 core

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

uniform ivec2 canvasSize; 
uniform int brushSize; 

// UV coords + brush radius;
out vec3 uvb;

vec4 canvas2view(vec2 p) {
	return vec4(2 * (p / canvasSize) - 1, 0, 1);
}

void main() {
	// Positions
	vec2 v0 = gl_in[0].gl_Position.xy;
	vec2 v1 = gl_in[1].gl_Position.xy;

	// Pressures
	float p0 = gl_in[0].gl_Position.z;
	float p1 = gl_in[1].gl_Position.z;

	vec2 d = v1 - v0;
	vec2 dn = dot(d, d) < 0.00001 ? vec2(1,0) : normalize(d);
	vec2 td = vec2(dn.y, -dn.x);

	float b0 = brushSize * p0;
	float b1 = brushSize * p1;

	gl_Position = canvas2view(v0 + b0*td - b0*dn);	uvb = vec3(0.0, 0.0, b0); EmitVertex();
	gl_Position = canvas2view(v0 - b0*td - b0*dn);	uvb = vec3(0.0, 1.0, b0); EmitVertex();
	gl_Position = canvas2view(v0 + b0*td);			uvb = vec3(0.5, 0.0, b0); EmitVertex();
	gl_Position = canvas2view(v0 - b0*td);			uvb = vec3(0.5, 1.0, b0); EmitVertex();
	gl_Position = canvas2view(v1 + b1*td);			uvb = vec3(0.5, 0.0, b1); EmitVertex();
	gl_Position = canvas2view(v1 - b1*td);			uvb = vec3(0.5, 1.0, b1); EmitVertex();
	gl_Position = canvas2view(v1 + b1*td + b1*dn);	uvb = vec3(1.0, 0.0, b1); EmitVertex();
	gl_Position = canvas2view(v1 - b1*td + b1*dn);	uvb = vec3(1.0, 1.0, b1); EmitVertex();

	EndPrimitive();
}