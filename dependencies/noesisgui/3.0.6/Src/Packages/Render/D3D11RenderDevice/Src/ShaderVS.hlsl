struct In
{
    float2 position: POSITION;

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
};

struct Out
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

#if GEN_ST1
    float2 st1: TEXCOORD4;
#endif
};

cbuffer Buffer0: register(b0)
{
    float4x4 projectionMtx;
}

cbuffer Buffer1: register(b1)
{
    float2 textureSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(in In i, out Out o)
{
    o.position = mul(float4(i.position, 0, 1), projectionMtx);

#if HAS_COLOR
    o.color = i.color;
#endif

#if HAS_UV0
    o.uv0 = i.uv0;
#endif

#if HAS_UV1
    o.uv1 = i.uv1;
#endif

#if HAS_UV2
    o.uv2 = i.uv2;
#endif

#if HAS_COVERAGE
    o.coverage = i.coverage;
#endif

#if GEN_ST1
    o.st1 = i.uv1 * textureSize.xy;
#endif
}