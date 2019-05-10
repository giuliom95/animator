#version 450 core

uniform sampler2D stroke;
uniform bool blur;

in vec2 uv;
out vec3 color;

float blurStroke() {
    ivec2 canvasSize = textureSize(stroke, 0);
    ivec2 texCoord = ivec2(uv * canvasSize);

    float c = 0;

    //c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, -2)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, -1)).r;
    //c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2,  0)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, +1)).r;
    //c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(-2, +2)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, -2)).r;
    //c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, -1)).r;
    //c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1,  0)).r;
    //c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, +1)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, +2)).r;
    //c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, -2)).r;
    //c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, -1)).r;
    //c +=  41 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0,  0)).r;
    //c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, +1)).r;
    //c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, +2)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, -2)).r;
    //c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, -1)).r;
    //c +=  26 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1,  0)).r;
    //c +=  16 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, +1)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, +2)).r;
    //c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, -2)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, -1)).r;
    //c +=   7 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2,  0)).r;
    //c +=   4 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, +1)).r;
    //c +=   1 * texelFetchOffset(stroke, texCoord, 0, ivec2(+2, +2)).r;
    //return c / 273;

    
    // c +=  1 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, -1)).r;
    // c +=  2 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1,  0)).r;
    // c +=  1 * texelFetchOffset(stroke, texCoord, 0, ivec2(-1, +1)).r;
    // c +=  2 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, -1)).r;
    // c +=  4 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0,  0)).r;
    // c +=  2 * texelFetchOffset(stroke, texCoord, 0, ivec2( 0, +1)).r;
    // c +=  1 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, -1)).r;
    // c +=  2 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1,  0)).r;
    // c +=  1 * texelFetchOffset(stroke, texCoord, 0, ivec2(+1, +1)).r;
    // return c / 16;

    for(int i = -2; i < 2; ++i) {
        for(int j = -2; j < 2; ++j) {
            ivec2 off = ivec2(i, j);
            float a = exp(-pow(length(off), 2));
            c += a * texelFetch(stroke, texCoord + off, 0).r;
        }
    }

    return c;
}

void main(){

    float v = blur ? blurStroke() : texture(stroke, uv).r;
    color = 1 + -vec3(v);
    
}