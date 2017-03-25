#include "Common.fx"

Texture2D<float4> GradientBuffer       : register(t0);
Texture2D<float4> shade0               : register(t1);
Texture2D<float4> shade1               : register(t2);
Texture2D<float4> shade2               : register(t3);
Texture2D<float4> shade3               : register(t4);
Texture2D<float4> shade4               : register(t5);
Texture2D<float4> shade5               : register(t6);

SamplerState      PointSampler                  : register(s0);
SamplerState      MirrorSampler                  : register(s1);
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBufferPerFrame : register(b0)
{
    matrix View;
    matrix Projection;
    float4 lightPosition;
    float2 ScreenSize;
    float StrokeRotation;
};

static const float2 BasePos[6] = {
    float2(-1.0f,  1.0f),
    float2(1.0f,  1.0f),
    float2(1.0f, -1.0f),
    float2(1.0f, -1.0f),
    float2(-1.0f, -1.0f),
    float2(-1.0f,  1.0f),
};

static const float2 UV[6] = {
    float2(0.0f, 0.0f), // Top Left
    float2(1.0f, 0.0f), // Top Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(0.0f, 1.0f), // Bottom Left
    float2(0.0f, 0.0f), // Top Left
};

cbuffer ConstantBufferPerObjectVS : register(b1)
{
    matrix World;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TextureUV : TEXCOORD0;
    uint id : SV_VertexID;
};

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float3 Normal : NORMAL0;
    float3 WorldPos : POSITION;
    float2 TextureUV : TEXCOORD0;
    float2 Direction0 : TEXCOORD1;
    float2 Direction1  : TEXCOORD2;
    float2 Direction2 : TEXCOORD3;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT Uniform_VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = float4(input.Pos, 1.0f);
    output.Pos = mul(output.Pos, World);
    output.WorldPos = output.Pos.xyz / output.Pos.w;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Normal = mul(float4(input.Normal, 0.0f), World).xyz;

    // position to vertex clip-space
    float4 fake_frag_coord = output.Pos;                // Range:   [-w,w]^4

                                                                            // vertex to NDC-space
    fake_frag_coord.x = fake_frag_coord.x / fake_frag_coord.w;  // Rescale: [-1,1]^3
    fake_frag_coord.y = fake_frag_coord.y / fake_frag_coord.w;  // Rescale: [-1,1]^3
    fake_frag_coord.z = fake_frag_coord.z / fake_frag_coord.w;  // Rescale: [-1,1]^3
    fake_frag_coord.w = 1.0 / fake_frag_coord.w;                                // Invert W

                                                                                // Vertex in window-space
    fake_frag_coord.x = fake_frag_coord.x * 0.5;
    fake_frag_coord.y = fake_frag_coord.y * 0.5;
    fake_frag_coord.z = fake_frag_coord.z * 0.5;

    fake_frag_coord.x = fake_frag_coord.x + 0.5;
    fake_frag_coord.y = fake_frag_coord.y + 0.5;
    fake_frag_coord.z = fake_frag_coord.z + 0.5;

    // Scale and Bias for Viewport (We want the window coordinates, so no need for this)
    fake_frag_coord.x = fake_frag_coord.x / ScreenSize.x;
    fake_frag_coord.y = fake_frag_coord.y / ScreenSize.y;

    fake_frag_coord = float4(get2dPoint(output.Pos, View, Projection, ScreenSize.x, ScreenSize.y), 0.0f, 0.0f);
    float4 unitypos = UnityObjectToClipPos(input.Pos, World * View * Projection);
    float4 screenPos = ComputeScreenPos(unitypos);
    float2 depthUV = screenPos.xy / screenPos.w;
    //float3 uOffsets = float3(-1, 0, 1) * _CameraDepthTexture_TexelSize.x;
    //float3 vOffsets = float3(-1, 0, 1) * _CameraDepthTexture_TexelSize.y;



    output.TextureUV.xy = input.TextureUV;
    fake_frag_coord.xy = normalize(fake_frag_coord.xy);
    fake_frag_coord.xy = depthUV;
    fake_frag_coord.xy = ScreenSpaceInVertexShader(output.Pos, ScreenSize);
    output.Direction0.x = GradientBuffer.SampleLevel(PointSampler, fake_frag_coord, 0).x;
    output.Direction0.y = GradientBuffer.SampleLevel(PointSampler, fake_frag_coord, 0).x;
    return output;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Uniform_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 screenSpacePosition = get2dPoint(input.Pos.xyz, View, Projection, ScreenSize.x, ScreenSize.y);
    screenSpacePosition /= ScreenSize;
    screenSpacePosition = input.Pos.xy;
    screenSpacePosition /= ScreenSize;
    //float2 screenSpacePosition = input.Pos.xy;
    float3 luminanceDirection = GradientBuffer.Sample(PointSampler, screenSpacePosition.xy).rgb;
    float luminance = luminanceDirection.r;
    float2 direction;
    direction.x = luminanceDirection.g;
    direction.y = luminanceDirection.b;
    direction = input.Direction0;
    float2 xy = screenSpacePosition.xy / float2(143.0f, 143.0f);
    //luminance = GradientBuffer.Sample(PointSampler, xy.xy).rgb;
    float strokeRotation = atan2(direction.y, direction.x);
    float2 sampleCoordinates = float2(xy.x*cos(strokeRotation) - xy.y * sin(strokeRotation), xy.x * sin(strokeRotation) + xy.y * cos(strokeRotation));
    float lowValue = 0.0f;
    float highValue = 1.0f;
    float blendFactor = 0.5f;
    /*if (luminance < 0.15f)
    {
        lowValue = 0.0f;
        highValue = shade0.Sample(MirrorSampler, sampleCoordinates);
        blendFactor = luminance / 0.15f;
    }
    else if (luminance < 0.3f)
    {
        lowValue = shade0.Sample(MirrorSampler, sampleCoordinates);
        highValue = shade1.Sample(MirrorSampler, sampleCoordinates);
        blendFactor = (luminance - .15f) / .15f;
    }
    else if (luminance < 0.45f)
    {
        lowValue = shade1.Sample(MirrorSampler, sampleCoordinates);
        highValue = shade2.Sample(MirrorSampler, sampleCoordinates);
        blendFactor = (luminance - 0.3f) / .15f;
    }
    else if (luminance < 0.6f)
    {
        lowValue = shade2.Sample(MirrorSampler, sampleCoordinates);
        highValue = shade3.Sample(MirrorSampler, sampleCoordinates);
        blendFactor = (luminance - 0.45f) / .15f;
    }
    else if (luminance < 0.75f)
    {
        lowValue = shade3.Sample(MirrorSampler, sampleCoordinates);
        highValue = shade4.Sample(MirrorSampler, sampleCoordinates);
        blendFactor = (luminance - 0.6f) / .15f;
    }
    else if (luminance < 0.9f)
    {
        lowValue = shade4.Sample(MirrorSampler, sampleCoordinates);
        highValue = shade5.Sample(MirrorSampler, sampleCoordinates);
        blendFactor = (luminance - 0.75f) / .15f;
    }
    else
    {
        lowValue = shade5.Sample(MirrorSampler, sampleCoordinates);
        highValue = 1.0f;
        blendFactor = (luminance - 0.9f) / .1f;
    }

    luminance = lerp(lowValue, highValue, blendFactor);*/
    return float4(luminance, direction.x, direction.y, 1.0f);
    //return float4(luminance, luminance, luminance, luminance);
}
