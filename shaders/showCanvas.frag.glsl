#version 450 core

uniform sampler2D stroke;
uniform sampler2DArray canvas;
uniform int currentFrameLayerIndex;

in vec2 uv;
out vec3 out_color;

void main(){
	
	// Start with white
	out_color = vec3(1);

	// Put canvas on
	vec4 canvas_color = texture(canvas, vec3(uv, currentFrameLayerIndex));
	out_color = (1 - canvas_color.a)*out_color + canvas_color.a*canvas_color.rgb;

	// Put brush on
	vec3 brush_color = vec3(0);
	float stroke_alpha = texture(stroke, uv).r;
	out_color = (1 - stroke_alpha)*out_color + stroke_alpha*brush_color;
}