#version 450 core

uniform sampler2D image;

in vec2 uv;
out vec3 color;

void main(){
    vec3 img = texture(image, uv).rgb;

    ivec2 canvas_size = textureSize(image, 0);

    vec2 diff = (canvas_size / 2) - (uv * canvas_size);
    float sqdist = dot(diff, diff);

    float brush_size = 200;
    float a = exp(-sqdist/(brush_size*brush_size));

    color = vec3(img.r, a, img.b);
}