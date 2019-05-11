#version 450 core

uniform sampler2D stroke;
uniform bool blur;

in vec2 uv;
out vec3 color;

float blurStroke() {
    ivec2 canvasSize = textureSize(stroke, 0);
    ivec2 texCoord = ivec2(uv * canvasSize);

    float c = 0;

    c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, -2)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, -1)).r;
    c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2,  0)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, +1)).r;
    c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, +2)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, -2)).r;
    c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, -1)).r;
    c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1,  0)).r;
    c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, +1)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, +2)).r;
    c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, -2)).r;
    c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, -1)).r;
    c +=  41 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0,  0)).r;
    c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, +1)).r;
    c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, +2)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, -2)).r;
    c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, -1)).r;
    c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1,  0)).r;
    c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, +1)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, +2)).r;
    c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, -2)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, -1)).r;
    c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2,  0)).r;
    c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, +1)).r;
    c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, +2)).r;

    return c / 273;
}

void main(){

    /*float v = blur ? blurStroke() : texture(stroke, uv).r;
    v *= .5;
    color = 1 + -vec3(v);*/
    
    color = vec3(texture(stroke, uv).r);
}