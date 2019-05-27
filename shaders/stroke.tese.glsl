#version 450 core

layout(isolines, equal_spacing, ccw) in;

vec2 hermite(	const float x, 
				const vec2 v0, 
				const vec2 v1, 
				const vec2 t0, 
				const vec2 t1) {
	const float x2 = x*x;
	const float x3 = x*x*x;
	return	v0 * (2*x3 - 3*x2 + 1) +
			v1 * (-2*x3 + 3*x2) +
			t0 * (x3 - 2*x2 + x) +
			t1 * (x3 - x2);
}

void main() {
	const float	x		= gl_TessCoord.x;
	const vec2	v0		= gl_in[0].gl_Position.xy;
	const vec2	v1		= gl_in[1].gl_Position.xy;
	const float	p0		= gl_in[0].gl_Position.z;
	const float	p1		= gl_in[1].gl_Position.z;
	const float	angle0	= gl_in[0].gl_Position.w;
	const float	angle1	= gl_in[1].gl_Position.w;
	const float	d		= length(v1 - v0); 
	const vec2	t0		= d*vec2(cos(angle0), sin(angle0));
	const vec2	t1		= d*vec2(cos(angle1), sin(angle1));

	gl_Position = vec4(
		hermite(x, v0, v1, t0, t1),
		(1-x)*p0 + x*p1,
		1
	);
}