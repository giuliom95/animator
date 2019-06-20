#version 450 core

uniform sampler2D stroke;
uniform sampler2DArray canvas;
uniform bool onionActive;
uniform bool eraserActive;
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

	vec4 pix = vec4(1,1,1,0);

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
			pix.rgb = (1 - frame_pix.a)*pix.rgb + frame_pix.a*frame_lum.rgb;
			pix.a = (1 - frame_pix.a)*pix.a + frame_pix.a;
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
			pix.rgb = (1 - frame_pix.a)*pix.rgb + frame_pix.a*frame_lum.rgb;
			pix.a = (1 - frame_pix.a)*pix.a + frame_pix.a;
		}
	}
	

	// Put canvas on
	vec4 frame_pix = texture(canvas, vec3(uv, currentFrameLayerIndex));
	pix.rgb = (1 - frame_pix.a)*pix.rgb + frame_pix.a*frame_pix.rgb;
	pix.a = (1 - frame_pix.a)*pix.a + frame_pix.a;

	// Put brush on
	vec3 brush_color = vec3(0);
	float stroke_alpha = texture(stroke, uv).r;
	pix.rgb = (1 - stroke_alpha)*pix.rgb + stroke_alpha*brush_color;
	pix.a = (1 - stroke_alpha)*pix.a + stroke_alpha;

	out_color = pix.a*pix.rgb + (1-pix.a)*vec3(1);
}