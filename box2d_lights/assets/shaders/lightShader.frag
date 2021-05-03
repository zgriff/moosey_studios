R"(

#ifdef CUGLES
// This one line is all the difference
precision highp float;  // highp required for gradient precision
#endif

// Bit vector for texturing, gradients, and scissoring
uniform int  uType;
// Blur offset for simple kernel blur
uniform vec2 uBlur;

// The texture for sampling
uniform sampler2D uTexture;

uniform vec4 uPlayers[8];

// The output color
out vec4 frag_color;

// The inputs from the vertex shader
in vec2 outPosition;
in vec4 outColor;
in vec2 outTexCoord;

float sq (float x) {
    return x * x;
}

float dist (vec2 v1, vec2 v2) {
    float d = sq(v1.x - v2.x) + sq(v1.y - v2.y);
    return sqrt(d);
}

// The stroke+gradient uniform block
layout (std140) uniform uContext
{
    mat3 scMatrix;      // 48
    vec2 scExtent;      //  8
    vec2 scScale;       //  8
    mat3 gdMatrix;      // 48
    vec4 gdInner;       // 16
    vec4 gdOuter;       // 16
    vec2 gdExtent;      //  8
    float gdRadius;     //  4
    float gdFeathr;     //  4
};

/**
 * Returns an interpolation value for a box gradient
 *
 * The value returned is the mix parameter for the inner and
 * outer color.
 *
 * Adapted from nanovg by Mikko Mononen (memon@inside.org)
 *
 * pt:      The (transformed) point to test
 * ext:     The gradient extent
 * radius:  The gradient radius
 * feather: The gradient feather amount
 */
float boxgradient(vec2 pt, vec2 ext, float radius, float feather) {
    vec2 ext2 = ext - vec2(radius,radius);
    vec2 dst = abs(pt) - ext2;
    float m = min(max(dst.x,dst.y),0.0) + length(max(dst,0.0)) - radius;
    return clamp((m + feather*0.5) / feather, 0.0, 1.0);
}

/**
 * Returns an alpha value for scissoring
 *
 * A pixel with value 0 is dropped, while one with value 1 is kept.
 * The scale value sets the 0 to 1 transition (which should be quick).
 *
 * Adapted from nanovg by Mikko Mononen (memon@inside.org)
 *
 * pt:  The point to test
 */
float scissormask(vec2 pt) {
    vec2 sc = (abs((scMatrix * vec3(pt,1.0)).xy) - scExtent);
    sc = vec2(0.5,0.5) - sc * scScale;
    return clamp(sc.x,0.0,1.0) * clamp(sc.y,0.0,1.0);
}

/**
 * Returns the result of a simple kernel blur
 *
 * This function blurs the texture with a simple 9-element
 * kernel.  It is not meant to provide a full-feature
 * Gaussian blur. It is a high performance blur for simple
 * effects (like font blurring).
 *
 * The idea for this function was taken from
 *     https://learnopengl.com/Advanced-OpenGL/Framebuffers
 *
 * coord: The texture coordinate to blur
 */
vec4 blursample(vec2 coord) {
    // Sample offsets
    vec2 off[9] = vec2[](
        vec2(-uBlur.x,  uBlur.y), // top-left
        vec2(    0.0f,  uBlur.y), // top-center
        vec2( uBlur.x,  uBlur.y), // top-right
        vec2(-uBlur.x,     0.0f), // center-left
        vec2(    0.0f,     0.0f), // center-center
        vec2( uBlur.x,     0.0f), // center-right
        vec2(-uBlur.x, -uBlur.y), // bottom-left
        vec2(    0.0f, -uBlur.y), // bottom-center
        vec2( uBlur.x, -uBlur.y)  // bottom-right
    );
    
    // Blur kernel
    float kernel[9] = float[](
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    );
    
    // Sample from the texture and average
    vec4 result = vec4(0.0);
    for(int ii = 0; ii < 9; ii++) {
        result += texture(uTexture, coord + off[ii])*kernel[ii];
    }

    return result;
}

vec3 playerLight(vec4 player, vec2 pos, vec3 texColor) {
    vec3 col = vec3(0.0, 0.0, 0.0);
    if (player.z == 0.0f) {
        return col;
    }
    //the flashlight angle
    float lightRadius = 300.0;
    float d = dist(player.xy, pos);
    vec3 flashCol = texColor;
    if (d <= lightRadius) {
        col = flashCol;
    }
    return col;

}


/**
 * Performs the main fragment shading.
 */
void main(void) {
    vec4 result = texture(uTexture, outTexCoord);
    vec3 col = vec3(0.0, 0.0, 0.0);
    
    for (int i = 0; i < 1; i++) {
        vec3 pLight = playerLight(uPlayers[i], outPosition, result.rgb);
        col += pLight;
    }

    frag_color = vec4(col, result.a);
}

)"
