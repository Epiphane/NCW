static const char* Shader_100es_frag = R"(#line 1

#if defined(EFFECT_RGBA)

#elif defined(EFFECT_MASK)

#elif defined(EFFECT_PATH_SOLID)
    #define EFFECT_PATH 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1

#elif defined(EFFECT_PATH_LINEAR)
    #define EFFECT_PATH 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1

#elif defined(EFFECT_PATH_RADIAL)
    #define EFFECT_PATH 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1

#elif defined(EFFECT_PATH_PATTERN)
    #define EFFECT_PATH 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1

#elif defined(EFFECT_PATH_AA_SOLID)
    #define EFFECT_PATH_AA 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_COVERAGE 1

#elif defined(EFFECT_PATH_AA_LINEAR)
    #define EFFECT_PATH_AA 1
    #define PAINT_LINEAR 1
    #define HAS_COVERAGE 1
    #define HAS_UV0 1

#elif defined(EFFECT_PATH_AA_RADIAL)
    #define EFFECT_PATH_AA 1
    #define PAINT_RADIAL 1
    #define HAS_COVERAGE 1
    #define HAS_UV0 1

#elif defined(EFFECT_PATH_AA_PATTERN)
    #define EFFECT_PATH_AA 1
    #define PAINT_PATTERN 1
    #define HAS_COVERAGE 1
    #define HAS_UV0 1

#elif defined(EFFECT_SDF_SOLID)
    #define EFFECT_SDF 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_ST1 1
    #extension GL_OES_standard_derivatives : enable

#elif defined(EFFECT_SDF_LINEAR)
    #define EFFECT_SDF 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1
    #extension GL_OES_standard_derivatives : enable

#elif defined(EFFECT_SDF_RADIAL)
    #define EFFECT_SDF 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1
    #extension GL_OES_standard_derivatives : enable

#elif defined(EFFECT_SDF_PATTERN)
    #define EFFECT_SDF 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1
    #extension GL_OES_standard_derivatives : enable

#elif defined(EFFECT_IMAGE_OPACITY_SOLID)
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1

#elif defined(EFFECT_IMAGE_OPACITY_LINEAR)
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1

#elif defined(EFFECT_IMAGE_OPACITY_RADIAL)
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1

#elif defined(EFFECT_IMAGE_OPACITY_PATTERN)
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1

#elif defined(EFFECT_IMAGE_SHADOW_35V)
    #define EFFECT_IMAGE_SHADOW_V 1
    #define GAUSSIAN_35_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_63V)
    #define EFFECT_IMAGE_SHADOW_V 1
    #define GAUSSIAN_63_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif defined(EFFECT_IMAGE_SHADOW_127V)
    #define EFFECT_IMAGE_SHADOW_V 1
    #define GAUSSIAN_127_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif defined(EFFECT_IMAGE_SHADOW_35H_SOLID)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_35H_LINEAR)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_35H_RADIAL)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_35H_PATTERN)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_63H_SOLID)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_63H_LINEAR)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_63H_RADIAL)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_63H_PATTERN)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_127H_SOLID)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_127H_LINEAR)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_127H_RADIAL)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_SHADOW_127H_PATTERN)
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_35V)
    #define EFFECT_IMAGE_BLUR_V 1
    #define GAUSSIAN_35_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_63V)
    #define EFFECT_IMAGE_BLUR_V 1
    #define GAUSSIAN_63_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif defined(EFFECT_IMAGE_BLUR_127V)
    #define EFFECT_IMAGE_BLUR_V 1
    #define GAUSSIAN_127_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif defined(EFFECT_IMAGE_BLUR_35H_SOLID)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_35H_LINEAR)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_35H_RADIAL)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_35H_PATTERN)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_63H_SOLID)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_63H_LINEAR)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_63H_RADIAL)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_63H_PATTERN)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_127H_SOLID)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_127H_LINEAR)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_127H_RADIAL)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif defined(EFFECT_IMAGE_BLUR_127H_PATTERN)
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#else
    #error EFFECT not defined
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
#define SDF_SCALE 7.96875
#define SDF_BIAS 0.50196078431
#define SDF_AA_FACTOR 0.65
#define SDF_BASE_MIN 0.125
#define SDF_BASE_MAX 0.25
#define SDF_BASE_DEV -0.65

precision mediump float;

#ifndef GL_FRAGMENT_PRECISION_HIGH
    #define highp mediump
#endif

uniform lowp vec4 rgba;
uniform lowp float opacity;
uniform vec4 radialGrad[2];
uniform vec2 textPixelSize;
uniform float effectParams[32];

uniform sampler2D pattern;
uniform sampler2D ramps;
uniform sampler2D image;
uniform sampler2D glyphs;
uniform sampler2D shadow;

#ifdef HAS_COLOR
    varying lowp vec4 color;
#endif

#ifdef HAS_UV0
  #ifdef PAINT_RADIAL
    varying mediump vec2 uv0;
  #else
 	varying highp vec2 uv0;
  #endif
#endif

#ifdef HAS_UV1
    varying highp vec2 uv1;
#endif

#ifdef HAS_UV2
    varying highp vec4 uv2;
#endif

#ifdef HAS_ST1
    varying highp vec2 st1;
#endif

#ifdef HAS_COVERAGE
    varying lowp float coverage;
#endif

#if defined(GAUSSIAN_35_TAP)
    #define GAUSSIAN_NUM_SAMPLES 9
    // No support for initializing arrays at declaration time in OpenGL ES 2
    #define SAMPLE(i, weight, offset) w[i] = weight; o[i] = offset;
    #define DECLARE_SAMPLES float w[GAUSSIAN_NUM_SAMPLES]; float o[GAUSSIAN_NUM_SAMPLES]; SAMPLE(0, 0.10855, 0.66293) SAMPLE(1, 0.13135, 2.47904) SAMPLE(2, 0.10406, 4.46232) SAMPLE(3, 0.07216, 6.44568) SAMPLE(4, 0.04380, 8.42917) SAMPLE(5, 0.02328, 10.41281) SAMPLE(6, 0.01083, 12.39664) SAMPLE(7, 0.00441, 14.38070) SAMPLE(8, 0.00157, 16.36501)
#endif

#if defined(GAUSSIAN_63_TAP)
    #define GAUSSIAN_NUM_SAMPLES 16
    // No support for initializing arrays at declaration time in OpenGL ES 2
    #define SAMPLE(i, weight, offset) w[i] = weight; o[i] = offset;
    #define DECLARE_SAMPLES float w[GAUSSIAN_NUM_SAMPLES]; float o[GAUSSIAN_NUM_SAMPLES]; SAMPLE(0, 0.05991, 0.66555) SAMPLE(1, 0.07758, 2.49371) SAMPLE(2, 0.07232, 4.48868) SAMPLE(3, 0.06476, 6.48366) SAMPLE(4, 0.05571, 8.47864) SAMPLE(5, 0.04604, 10.47362) SAMPLE(6, 0.03655, 12.46860) SAMPLE(7, 0.02788, 14.46360) SAMPLE(8, 0.02042, 16.45860) SAMPLE(9, 0.01438, 18.45361) SAMPLE(10, 0.00972, 20.44863) SAMPLE(11, 0.00631, 22.44365) SAMPLE(12, 0.00394, 24.43869) SAMPLE(13, 0.00236, 26.43375) SAMPLE(14, 0.00136, 28.42881) SAMPLE(15, 0.00075, 30.42389)
#endif

#if defined(GAUSSIAN_127_TAP)
    #define GAUSSIAN_NUM_SAMPLES 32
    // No support for initializing arrays at declaration time in OpenGL ES 2
    #define SAMPLE(i, weight, offset) w[i] = weight; o[i] = offset;
    #define DECLARE_SAMPLES float w[GAUSSIAN_NUM_SAMPLES]; float o[GAUSSIAN_NUM_SAMPLES]; SAMPLE(0, 0.02954, 0.66640) SAMPLE(1, 0.03910, 2.49848) SAMPLE(2, 0.03844, 4.49726) SAMPLE(3, 0.03743, 6.49605) SAMPLE(4, 0.03609, 8.49483) SAMPLE(5, 0.03446, 10.49362) SAMPLE(6, 0.03259, 12.49240) SAMPLE(7, 0.03052, 14.49119) SAMPLE(8, 0.02830, 16.48997) SAMPLE(9, 0.02600, 18.48876) SAMPLE(10, 0.02365, 20.48754) SAMPLE(11, 0.02130, 22.48633) SAMPLE(12, 0.01900, 24.48511) SAMPLE(13, 0.01679, 26.48390) SAMPLE(14, 0.01469, 28.48268) SAMPLE(15, 0.01272, 30.48147) SAMPLE(16, 0.01092, 32.48026) SAMPLE(17, 0.00928, 34.47904) SAMPLE(18, 0.00781, 36.47783) SAMPLE(19, 0.00651, 38.47662) SAMPLE(20, 0.00537, 40.47540) SAMPLE(21, 0.00439, 42.47419) SAMPLE(22, 0.00355, 44.47298) SAMPLE(23, 0.00285, 46.47176) SAMPLE(24, 0.00226, 48.47055) SAMPLE(25, 0.00178, 50.46934) SAMPLE(26, 0.00138, 52.46813) SAMPLE(27, 0.00107, 54.46692) SAMPLE(28, 0.00081, 56.46571) SAMPLE(29, 0.00062, 58.46450) SAMPLE(30, 0.00046, 60.46329) SAMPLE(31, 0.00034, 62.46208)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    /////////////////////////////////////////////////////
    // Fetch paint color and opacity
    /////////////////////////////////////////////////////
    #if defined(PAINT_SOLID)
        lowp vec4 paint = color;
        lowp float opacity_ = 1.0;

    #elif defined(PAINT_LINEAR)
        lowp vec4 paint = texture2D(ramps, uv0);
        lowp float opacity_ = opacity;

    #elif defined(PAINT_RADIAL)
        float dd = radialGrad[1].y * uv0.x - radialGrad[1].z * uv0.y;
        float u = radialGrad[0].x * uv0.x + radialGrad[0].y * uv0.y + radialGrad[0].z * 
            sqrt(uv0.x * uv0.x + uv0.y * uv0.y - dd * dd);
        lowp vec4 paint = texture2D(ramps, vec2(u, radialGrad[1].w));
        lowp float opacity_ = opacity;

    #elif defined(PAINT_PATTERN)
        lowp vec4 paint = texture2D(pattern, uv0);
        lowp float opacity_ = opacity;
    #endif

    /////////////////////////////////////////////////////
    // Apply selected effect
    /////////////////////////////////////////////////////
    #if defined(EFFECT_RGBA)
        gl_FragColor = rgba;

    #elif defined(EFFECT_MASK)
        gl_FragColor = vec4(1);

    #elif defined(EFFECT_PATH)
        gl_FragColor = opacity_ * paint;

    #elif defined(EFFECT_PATH_AA)
        gl_FragColor = (opacity_ * coverage) * paint;

    #elif defined(EFFECT_IMAGE_OPACITY)
        gl_FragColor = texture2D(image, uv1) * (opacity_ * paint.a);

    #elif defined(EFFECT_IMAGE_SHADOW_V)
        #define BLUR_SIZE effectParams[0]

        float alpha = 0.0;
        float dir = BLUR_SIZE * textPixelSize.y;

        DECLARE_SAMPLES

        for (int i = 0; i < GAUSSIAN_NUM_SAMPLES; i++)
        {
            float offset = o[i] * dir;
            vec2 up = vec2(uv1.x, min(uv1.y + offset, uv2.w));
            vec2 down = vec2(uv1.x, max(uv1.y - offset, uv2.y)); 
            alpha += w[i] * (texture2D(image, up).a + texture2D(image, down).a);
        }

        gl_FragColor = vec4(0, 0, 0, alpha);

    #elif defined(EFFECT_IMAGE_SHADOW_H)
        #define SHADOW_COLOR vec4(effectParams[0], effectParams[1], effectParams[2], effectParams[3])
        #define BLUR_SIZE effectParams[4]
        #define SHADOW_OFFSETX effectParams[5]
        #define SHADOW_OFFSETY -effectParams[6]

        float alpha = 0.0;
        vec2 dir = vec2(BLUR_SIZE * textPixelSize.x, 0);
        vec2 offset = vec2(SHADOW_OFFSETX * textPixelSize.x, SHADOW_OFFSETY * textPixelSize.y);

        DECLARE_SAMPLES

        for (int i = 0; i < GAUSSIAN_NUM_SAMPLES; i++)
        {
            vec2 uvOffset = o[i] * dir;
            alpha += w[i] * (texture2D(shadow, clamp(uv1 - offset + uvOffset, uv2.xy, uv2.zw)).a + texture2D(shadow, clamp(uv1 - offset - uvOffset, uv2.xy, uv2.zw)).a);
        }

        lowp vec4 img = texture2D(image, clamp(uv1, uv2.xy, uv2.zw));
        gl_FragColor = (img + (1.0 - img.a) * (SHADOW_COLOR * alpha)) * (opacity_ * paint.a);

    #elif defined(EFFECT_IMAGE_BLUR_V)
        #define BLUR_SIZE effectParams[0]

        vec4 color = vec4(0);
        float dir = BLUR_SIZE * textPixelSize.y;

        DECLARE_SAMPLES

        for (int i = 0; i < GAUSSIAN_NUM_SAMPLES; i++)
        {
            float offset = o[i] * dir;
			vec2 up = vec2(uv1.x, min(uv1.y + offset, uv2.w));
            vec2 down = vec2(uv1.x, max(uv1.y - offset, uv2.y));
            color += w[i] * (texture2D(image, up) + texture2D(image, down));
        }

        gl_FragColor = color;

    #elif defined(EFFECT_IMAGE_BLUR_H)
        #define BLUR_SIZE effectParams[0]

        vec4 color = vec4(0);
        float dir = BLUR_SIZE * textPixelSize.x;

        DECLARE_SAMPLES

        for (int i = 0; i < GAUSSIAN_NUM_SAMPLES; i++)
        {
            float offset = o[i] * dir;
            vec2 right = vec2(min(uv1.x + offset, uv2.z), uv1.y);
            vec2 left = vec2(max(uv1.x - offset, uv2.x), uv1.y);
            color += w[i] * (texture2D(image, right) + texture2D(image, left));
        }

        gl_FragColor = color * (opacity_ * paint.a);

    #elif defined(EFFECT_SDF)
        vec4 color = texture2D(glyphs, uv1);
        float distance = SDF_SCALE * (color.r - SDF_BIAS);
        vec2 grad = dFdx(st1);

        float gradLen = length(grad);
        float scale = 1.0 / gradLen;
        float base = SDF_BASE_DEV * (1.0 - (clamp(scale, SDF_BASE_MIN, SDF_BASE_MAX) - SDF_BASE_MIN) / (SDF_BASE_MAX - SDF_BASE_MIN));
        float range = SDF_AA_FACTOR * gradLen;
        lowp float alpha = smoothstep(base - range, base + range, distance);
        gl_FragColor = (alpha * opacity_) * paint;

    #endif
}

)";