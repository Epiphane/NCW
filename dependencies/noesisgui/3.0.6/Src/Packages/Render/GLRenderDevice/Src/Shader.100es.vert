#version 100

attribute vec2 attr_pos;

#ifdef HAS_COLOR
    attribute lowp vec4 attr_color;
    varying lowp vec4 color;
#endif

#ifdef HAS_UV0
    attribute vec2 attr_tex0;
    varying vec2 uv0;
#endif

#ifdef HAS_UV1
    attribute vec2 attr_tex1;
    varying vec2 uv1;
#endif

#ifdef HAS_UV2
    attribute vec4 attr_tex2;
    varying vec4 uv2;
#endif

#ifdef HAS_ST1
    varying vec2 st1;
#endif

#ifdef HAS_COVERAGE
    attribute lowp float attr_coverage;
    varying lowp float coverage;
#endif

uniform mat4 projectionMtx;
uniform vec2 textSize;

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    gl_Position = vec4(attr_pos, 0, 1) * projectionMtx;

#ifdef HAS_COLOR
    color = attr_color;
#endif

#ifdef HAS_UV0
    uv0 = attr_tex0;
#endif

#ifdef HAS_UV1
    uv1 = attr_tex1;
#endif

#ifdef HAS_UV2
    uv2 = attr_tex2;
#endif

#ifdef HAS_ST1
    st1 = attr_tex1 * textSize.xy;
#endif

#ifdef HAS_COVERAGE
    coverage = attr_coverage;
#endif
}