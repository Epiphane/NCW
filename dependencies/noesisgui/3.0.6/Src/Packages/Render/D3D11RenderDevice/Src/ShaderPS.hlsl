#if EFFECT_RGBA

#elif EFFECT_MASK

#elif EFFECT_PATH_SOLID
    #define EFFECT_PATH 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1

#elif EFFECT_PATH_LINEAR
    #define EFFECT_PATH 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1

#elif EFFECT_PATH_RADIAL
    #define EFFECT_PATH 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1

#elif EFFECT_PATH_PATTERN
    #define EFFECT_PATH 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1

#elif EFFECT_PATH_AA_SOLID
    #define EFFECT_PATH_AA 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_COVERAGE 1

#elif EFFECT_PATH_AA_LINEAR
    #define EFFECT_PATH_AA 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_COVERAGE 1

#elif EFFECT_PATH_AA_RADIAL
    #define EFFECT_PATH_AA 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_COVERAGE 1

#elif EFFECT_PATH_AA_PATTERN
    #define EFFECT_PATH_AA 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_COVERAGE 1

#elif EFFECT_SDF_SOLID
    #define EFFECT_SDF 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_LINEAR
    #define EFFECT_SDF 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_RADIAL
    #define EFFECT_SDF 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_PATTERN
    #define EFFECT_SDF 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_LCD_SOLID
    #define EFFECT_SDF_LCD 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_LCD_LINEAR
    #define EFFECT_SDF_LCD 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_LCD_RADIAL
    #define EFFECT_SDF_LCD 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_SDF_LCD_PATTERN
    #define EFFECT_SDF_LCD 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_ST1 1

#elif EFFECT_IMAGE_OPACITY_SOLID
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1

#elif EFFECT_IMAGE_OPACITY_LINEAR
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1

#elif EFFECT_IMAGE_OPACITY_RADIAL
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1

#elif EFFECT_IMAGE_OPACITY_PATTERN
    #define EFFECT_IMAGE_OPACITY 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1

#elif EFFECT_IMAGE_SHADOW_35V
    #define EFFECT_IMAGE_SHADOW_V 1
    #define GAUSSIAN_35_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_63V
    #define EFFECT_IMAGE_SHADOW_V 1
    #define GAUSSIAN_63_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif EFFECT_IMAGE_SHADOW_127V
    #define EFFECT_IMAGE_SHADOW_V 1
    #define GAUSSIAN_127_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif EFFECT_IMAGE_SHADOW_35H_SOLID
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_35H_LINEAR
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_35H_RADIAL
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_35H_PATTERN
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_63H_SOLID
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_63H_LINEAR
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_63H_RADIAL
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_63H_PATTERN
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_127H_SOLID
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_127H_LINEAR
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_127H_RADIAL
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_SHADOW_127H_PATTERN
    #define EFFECT_IMAGE_SHADOW_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_35V
    #define EFFECT_IMAGE_BLUR_V 1
    #define GAUSSIAN_35_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_63V
    #define EFFECT_IMAGE_BLUR_V 1
    #define GAUSSIAN_63_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif EFFECT_IMAGE_BLUR_127V
    #define EFFECT_IMAGE_BLUR_V 1
    #define GAUSSIAN_127_TAP 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 1

#elif EFFECT_IMAGE_BLUR_35H_SOLID
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_35H_LINEAR
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_35H_RADIAL
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_35H_PATTERN
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_35_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_63H_SOLID
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_63H_LINEAR
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_63H_RADIAL
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_63H_PATTERN
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_63_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_127H_SOLID
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_SOLID 1
    #define HAS_COLOR 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_127H_LINEAR
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_LINEAR 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_127H_RADIAL
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_RADIAL 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#elif EFFECT_IMAGE_BLUR_127H_PATTERN
    #define EFFECT_IMAGE_BLUR_H 1
    #define GAUSSIAN_127_TAP 1
    #define PAINT_PATTERN 1
    #define HAS_UV0 1
    #define HAS_UV1 1
    #define HAS_UV2 2

#else
    #error EFFECT not defined
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SDF_SCALE 7.96875
#define SDF_BIAS 0.50196078431
#define SDF_AA_FACTOR 0.65
#define SDF_BASE_MIN 0.125
#define SDF_BASE_MAX 0.25
#define SDF_BASE_DEV -0.65

Texture2D pattern: register(t0);
SamplerState patternSampler: register(s0);

Texture2D ramps: register(t1);
SamplerState rampsSampler: register(s1);

Texture2D image: register(t2);
SamplerState imageSampler: register(s2);

Texture2D glyphs: register(t3);
SamplerState glyphsSampler: register(s3);

Texture2D shadow: register(t4);
SamplerState shadowSampler: register(s4);

cbuffer Buffer0: register(b0)
{
#if EFFECT_RGBA
    float4 rgba;
#endif

#if PAINT_RADIAL
    float4 radialGrad[2];
#endif

#if PAINT_LINEAR || PAINT_RADIAL || PAINT_PATTERN
    float opacity;
#endif
}

cbuffer Buffer1: register(b1)
{
    float4 textureSize;
}

cbuffer Buffer2: register(b2)
{
#if EFFECT_IMAGE_SHADOW_V || EFFECT_IMAGE_BLUR_H || EFFECT_IMAGE_BLUR_V
    float blurSize;
#endif

#if EFFECT_IMAGE_SHADOW_H
    float4 shadowColor;
    float1 blurSize;
    float2 shadowOffset;
#endif
}

struct In
{
    float4 position: SV_Position;

#if HAS_COLOR
    half4 color: COLOR;
#endif

#if HAS_UV0
    float2 uv0: TEXCOORD0;
#endif

#if HAS_UV1
    float2 uv1: TEXCOORD1;
#endif

#if HAS_UV2
    float4 uv2: TEXCOORD2;
#endif

#if HAS_COVERAGE
    half coverage: TEXCOORD3;
#endif

#if HAS_ST1
    float2 st1: TEXCOORD4;
#endif
};

struct Out
{
    half4 color: SV_Target0;

#if EFFECT_SDF_LCD
    half4 alpha: SV_Target1;
#endif
};

#if GAUSSIAN_35_TAP
    #define GAUSSIAN_NUM_SAMPLES 9
    static const half weights[GAUSSIAN_NUM_SAMPLES] =
    {
        0.10855, 0.13135, 0.10406, 0.07216, 0.04380, 0.02328, 0.01083, 0.00441, 0.00157
    };
    static const half offsets[GAUSSIAN_NUM_SAMPLES] =
    {
        0.66293, 2.47904, 4.46232, 6.44568, 8.42917, 10.41281, 12.39664, 14.38070, 16.36501
    };
#endif

#if GAUSSIAN_63_TAP
    #define GAUSSIAN_NUM_SAMPLES 16
    static const half weights[GAUSSIAN_NUM_SAMPLES] =
    {
        0.05991, 0.07758, 0.07232, 0.06476, 0.05571, 0.04604, 0.03655, 0.02788, 0.02042, 0.01438,
        0.00972, 0.00631, 0.00394, 0.00236, 0.00136, 0.00075
    };
    static const half offsets[GAUSSIAN_NUM_SAMPLES] =
    {
        0.66555, 2.49371, 4.48868, 6.48366, 8.47864, 10.47362, 12.46860, 14.46360, 16.45860, 18.45361,
        20.44863, 22.44365, 24.43869, 26.43375, 28.42881, 30.42389
    };
#endif

#if GAUSSIAN_127_TAP
    #define GAUSSIAN_NUM_SAMPLES 32
    static const half weights[GAUSSIAN_NUM_SAMPLES] =
    {
        0.02954, 0.03910, 0.03844, 0.03743, 0.03609, 0.03446, 0.03259, 0.03052, 0.02830, 0.02600,
        0.02365, 0.02130, 0.01900, 0.01679, 0.01469, 0.01272, 0.01092, 0.00928, 0.00781, 0.00651,
        0.00537, 0.00439, 0.00355, 0.00285, 0.00226, 0.00178, 0.00138, 0.00107, 0.00081, 0.00062,
        0.00046, 0.00034
    };
    static const half offsets[GAUSSIAN_NUM_SAMPLES] =
    {
        0.66640, 2.49848, 4.49726, 6.49605, 8.49483, 10.49362, 12.49240, 14.49119, 16.48997, 18.48876,
        20.48754, 22.48633, 24.48511, 26.48390, 28.48268, 30.48147, 32.48026, 34.47904, 36.47783, 38.47662,
        40.47540, 42.47419, 44.47298, 46.47176, 48.47055, 50.46934, 52.46813, 54.46692, 56.46571, 58.46450,
        60.46329, 62.46208
    };
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Out main(in In i)
{
    /////////////////////////////////////////////////////
    // Fetch paint color and opacity
    /////////////////////////////////////////////////////
    #if PAINT_SOLID
        half4 paint = i.color;
        half opacity_ = 1.0f;

    #elif PAINT_LINEAR
        half4 paint = ramps.Sample(rampsSampler, i.uv0);
        half opacity_ = opacity;

    #elif PAINT_RADIAL
        half dd = radialGrad[1].y * i.uv0.x - radialGrad[1].z * i.uv0.y;
        half u = radialGrad[0].x * i.uv0.x + radialGrad[0].y * i.uv0.y + radialGrad[0].z *
            sqrt(radialGrad[0].w * i.uv0.x * i.uv0.x + radialGrad[1].x * i.uv0.y * i.uv0.y - dd * dd);
        half4 paint = ramps.Sample(rampsSampler, half2(u, radialGrad[1].w));
        half opacity_ = opacity;

    #elif PAINT_PATTERN
        half4 paint = pattern.Sample(patternSampler, i.uv0);
        half opacity_ = opacity;
    #endif

    Out o;

    /////////////////////////////////////////////////////
    // Apply selected effect
    /////////////////////////////////////////////////////
    #if EFFECT_RGBA
        o.color = rgba;

    #elif EFFECT_MASK
        o.color = half4(1, 1, 1, 1);

    #elif EFFECT_PATH
        o.color = opacity_ * paint;

    #elif EFFECT_PATH_AA
        o.color = (opacity_ * i.coverage) * paint;

    #elif EFFECT_IMAGE_OPACITY
        o.color = image.Sample(imageSampler, i.uv1) * (opacity_ * paint.a);

    #elif EFFECT_IMAGE_SHADOW_V
        half alpha = 0;
        half2 dir = half2(0, blurSize * textureSize.w);
    
        for (int x = 0; x < GAUSSIAN_NUM_SAMPLES; x++)
        {
            half2 uvOffset = offsets[x] * dir;
            alpha += weights[x] * 
            (
                image.Sample(imageSampler, clamp(i.uv1 + uvOffset, i.uv2.xy, i.uv2.zw)).a + 
                image.Sample(imageSampler, clamp(i.uv1 - uvOffset, i.uv2.xy, i.uv2.zw)).a
            );
        }

        o.color = half4(0, 0, 0, alpha);

    #elif EFFECT_IMAGE_SHADOW_H
        half alpha = 0;
        half2 dir = half2(blurSize * textureSize.z, 0);
        half2 offset = half2(shadowOffset.x * textureSize.z, shadowOffset.y * textureSize.w);

        for (int x = 0; x < GAUSSIAN_NUM_SAMPLES; x++)
        {
            half2 uvOffset = offsets[x] * dir;
            alpha += weights[x] * 
            (
                shadow.Sample(shadowSampler, clamp(i.uv1 - offset + uvOffset, i.uv2.xy, i.uv2.zw)).a +
                shadow.Sample(shadowSampler, clamp(i.uv1 - offset - uvOffset, i.uv2.xy, i.uv2.zw)).a
            );
        }

        half4 img = image.Sample(imageSampler, clamp(i.uv1, i.uv2.xy, i.uv2.zw));
        o.color = (img + (1.0f - img.a) * (shadowColor * alpha)) * (opacity_ * paint.a);

    #elif EFFECT_IMAGE_BLUR_V
        half4 color = half4(0, 0, 0, 0);
        half2 dir = half2(0, blurSize * textureSize.w);
    
        for (int x = 0; x < GAUSSIAN_NUM_SAMPLES; x++)
        {
            half2 uvOffset = offsets[x] * dir;
            color += weights[x] * 
            (
                image.Sample(imageSampler, clamp(i.uv1 + uvOffset, i.uv2.xy, i.uv2.zw)) + 
                image.Sample(imageSampler, clamp(i.uv1 - uvOffset, i.uv2.xy, i.uv2.zw))
            );
        }

        o.color = color;

    #elif EFFECT_IMAGE_BLUR_H
        half4 color = half4(0, 0, 0, 0);
        half2 dir = half2(blurSize * textureSize.z, 0);
    
        for (int x = 0; x < GAUSSIAN_NUM_SAMPLES; x++)
        {
            half2 uvOffset = offsets[x] * dir;
            color += weights[x] * 
            (
                image.Sample(imageSampler, clamp(i.uv1 + uvOffset, i.uv2.xy, i.uv2.zw)) +
                image.Sample(imageSampler, clamp(i.uv1 - uvOffset, i.uv2.xy, i.uv2.zw))
            );
        }

        o.color = color * (opacity_ * paint.a);

    #elif EFFECT_SDF
        half4 color = glyphs.Sample(glyphsSampler, i.uv1);
        half distance = SDF_SCALE * (color.r - SDF_BIAS);

        #if 1
            half2 grad = ddx(i.st1);
        #else
            // For non-uniform scale or perspective this is the correct code. It is much more complex than the isotropic
            // case and probably not worth it
            // http://www.essentialmath.com/GDC2015/VanVerth_Jim_DrawingAntialiasedEllipse.pdf
            // https://www.essentialmath.com/blog/?p=151
            half2 Jdx = ddx(i.st1);
            half2 Jdy = ddy(i.st1);
            half2 distGrad = half2(ddx(distance), ddy(distance));
            half distGradLen2 = dot(distGrad, distGrad);
            distGrad = distGradLen2 < 0.0001 ? half2(0.7071, 0.7071) : distGrad * half(rsqrt(distGradLen2));
            half2 grad = half2(distGrad.x * Jdx.x + distGrad.y * Jdy.x, distGrad.x * Jdx.y + distGrad.y * Jdy.y);
        #endif

        half gradLen = (half)length(grad);
        half scale = 1.0 / gradLen;
        half base = SDF_BASE_DEV * (1.0f - (clamp(scale, SDF_BASE_MIN, SDF_BASE_MAX) - SDF_BASE_MIN) / (SDF_BASE_MAX - SDF_BASE_MIN));
        half range = SDF_AA_FACTOR * gradLen;
        half alpha = smoothstep(base - range, base + range, distance);

        o.color = (alpha * opacity_) * paint;

    #elif EFFECT_SDF_LCD
        half2 grad = ddx(i.st1);
        half2 offset = grad * textureSize.zw * (1.0 / 3.0);

        half4 red = glyphs.Sample(glyphsSampler, i.uv1 - offset);
        half4 green = glyphs.Sample(glyphsSampler, i.uv1);
        half4 blue = glyphs.Sample(glyphsSampler, i.uv1 + offset);
        half3 distance = SDF_SCALE * (half3(red.r, green.r, blue.r) - SDF_BIAS);

        half gradLen = (half)length(grad);
        half scale = 1.0 / gradLen;
        half base = SDF_BASE_DEV * (1.0 - (clamp(scale, SDF_BASE_MIN, SDF_BASE_MAX) - SDF_BASE_MIN) / (SDF_BASE_MAX - SDF_BASE_MIN));
        half range = SDF_AA_FACTOR * gradLen;
        half3 alpha = smoothstep(base - range, base + range, distance);

        o.color = half4(opacity_ * paint.rgb * alpha.rgb, alpha.g);
        o.alpha = half4((opacity_ * paint.a) * alpha.rgb, alpha.g);

    #endif

    return o;
}
