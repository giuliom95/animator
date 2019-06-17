#version 450 core

uniform sampler2D stroke;
uniform sampler2DArray canvas;
uniform bool onionActive;
uniform int currentFrameLayerIndex;
uniform ivec3 lowerCurrentUpperFrame;

in vec2 uv;
out vec3 out_color;

int cycle(int idx, int size) {
	if(idx < 0)
		return size - (abs(idx) % size);
	return idx % size;
}

float luma(vec3 c) {
	return 0.2126*c.r + 0.7152*c.g + 0.0722*c.b;
}

void main(){
	int layers = 3;
	int loadedFrames = 2*layers + 1;

	// Start with white
	out_color = vec3(1);

	if(onionActive) {
		// Frames before (BLUE)
		for(int i = 0; i < layers; ++i) {
			int ci = i - layers;
			int f = lowerCurrentUpperFrame.y + ci;
			if(f < lowerCurrentUpperFrame.x || f > lowerCurrentUpperFrame.z) continue;

			int fi = cycle(currentFrameLayerIndex + ci, loadedFrames);
			vec4 frame_pix = texture(canvas, vec3(uv, fi));
			vec3 frame_lum = vec3(luma(frame_pix.rgb));
			frame_lum.b = 1;
			frame_pix.a *= 0.5 * ((i+1) / float(layers));
			out_color = (1 - frame_pix.a)*out_color + frame_pix.a*frame_lum.rgb;
		}
		
		// Frames after (RED)
		for(int i = 0; i < layers; ++i) {
			int ci = i + 1;
			int f = lowerCurrentUpperFrame.y + ci;
			if(f < lowerCurrentUpperFrame.x || f > lowerCurrentUpperFrame.z) continue;

			int fi = cycle(currentFrameLayerIndex + ci, loadedFrames);
			vec4 frame_pix = texture(canvas, vec3(uv, fi));
			vec3 frame_lum = vec3(luma(frame_pix.rgb));
			frame_lum.r = 1;
			frame_pix.a *= 0.5 * (1 - i / float(layers));
			out_color = (1 - frame_pix.a)*out_color + frame_pix.a*frame_lum.rgb;
		}
	}
	

	// Put canvas on
	vec4 frame_pix = texture(canvas, vec3(uv, currentFrameLayerIndex));
	out_color = (1 - frame_pix.a)*out_color + frame_pix.a*frame_pix.rgb;

	// Put brush on
	vec3 brush_color = vec3(0);
	float stroke_alpha = texture(stroke, uv).r;
	out_color = (1 - stroke_alpha)*out_color + stroke_alpha*brush_color;
}