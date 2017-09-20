#include "Common.fx"


Texture2D<float4> StrokeDirectionBuffer : register(t0);
Texture2D<float4> LuminanceBuffer      : register(t1);
Texture2D<float4> shade0               : register(t2);
Texture2D<float4> shade1               : register(t3);
Texture2D<float4> shade2               : register(t4);
Texture2D<float4> shade3               : register(t5);
Texture2D<float4> shade4               : register(t6);
Texture2D<float4> shade5               : register(t7);

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

// Buffer visualizer (draw full screen rect) vertex output
struct VS_OUTPUT
{
    float4 Pos        : SV_POSITION;
    float2 TextureUV  : TEXCOORD0;
};

float2 GetSampleCoordinatesFromDirection(float2 direction, float2 screenSpacePosition, float2 screenSize)
{
    if (direction.x < FLT_EPSILON && direction.y < FLT_EPSILON && direction.x >= 0.0 && direction.y >= 0.0)
    {
        // Force a direciton if none was set in the gradient buffer
        direction.x = 0.0;
        direction.y = 1.0; // straight up and down so it's somewhat clear when debugging
    }

    direction.xy = normalize(direction);

    float2 xy = screenSpacePosition.xy * screenSize / float2(143.0f, 143.0f);

    float strokeRotation = (atan2(direction.y, direction.x) + PI) * 2.0f;
    float2 sampleCoordinates = float2(xy.x*cos(strokeRotation) - xy.y * sin(strokeRotation), xy.x * sin(strokeRotation) + xy.y * cos(strokeRotation));
    return sampleCoordinates;
}

float2 GetSampleCoordinatesFromRotation(float strokeRotation, float2 screenSpacePosition, float2 screenSize, float2 strokeTextureSize)
{
    float strokeRotationInRadians = strokeRotation * PI * 2.0;
    float2 xy = screenSpacePosition.xy * screenSize / strokeTextureSize;
    return float2(xy.x*cos(strokeRotationInRadians) - xy.y * sin(strokeRotationInRadians), xy.x * sin(strokeRotationInRadians) + xy.y * cos(strokeRotationInRadians));
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 RenderStrokes_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 screenSpacePosition = input.Pos.xy;
    screenSpacePosition /= ScreenSize;

    float4 luminanceBuffer = LuminanceBuffer.Sample(PointSampler, screenSpacePosition.xy);
    float luminance = luminanceBuffer.r;

    float4 directions = StrokeDirectionBuffer.Sample(PointSampler, screenSpacePosition.xy);
    float2 strokeTextureSize = float2(143.0f, 143.0f);
    float2 sampleCoordinates0 = GetSampleCoordinatesFromRotation(directions.x, screenSpacePosition, ScreenSize, strokeTextureSize);
    float2 sampleCoordinates1 = GetSampleCoordinatesFromRotation(directions.y, screenSpacePosition, ScreenSize, strokeTextureSize);
    float2 sampleCoordinates2 = GetSampleCoordinatesFromRotation(directions.z, screenSpacePosition, ScreenSize, strokeTextureSize);
    
    float lowValue = 0.0f;
    float highValue = 1.0f;
    float blendFactor = 0.5f;
    if (luminance < 0.15f)
    {
        lowValue = 0.0f;
        highValue = (1.0 / 3.0) * (shade0.Sample(MirrorSampler, sampleCoordinates0) + shade0.Sample(MirrorSampler, sampleCoordinates1) + shade0.Sample(MirrorSampler, sampleCoordinates2));
        blendFactor = luminance / 0.15f;
    }
    else if (luminance < 0.3f)
    {
        lowValue = (1.0 / 3.0) * (shade0.Sample(MirrorSampler, sampleCoordinates0) + shade0.Sample(MirrorSampler, sampleCoordinates1) + shade0.Sample(MirrorSampler, sampleCoordinates2));
        highValue = (1.0 / 3.0) * (shade1.Sample(MirrorSampler, sampleCoordinates0) + shade1.Sample(MirrorSampler, sampleCoordinates1) + shade1.Sample(MirrorSampler, sampleCoordinates2));
        blendFactor = (luminance - .15f) / .15f;
    }
    else if (luminance < 0.45f)
    {
        lowValue = (1.0 / 3.0) * (shade1.Sample(MirrorSampler, sampleCoordinates0) + shade1.Sample(MirrorSampler, sampleCoordinates1) + shade1.Sample(MirrorSampler, sampleCoordinates2));
        highValue = (1.0 / 3.0) * (shade2.Sample(MirrorSampler, sampleCoordinates0) + shade2.Sample(MirrorSampler, sampleCoordinates1) + shade2.Sample(MirrorSampler, sampleCoordinates2));
        blendFactor = (luminance - 0.3f) / .15f;
    }
    else if (luminance < 0.6f)
    {
        lowValue = (1.0 / 3.0) * (shade2.Sample(MirrorSampler, sampleCoordinates0) + shade2.Sample(MirrorSampler, sampleCoordinates1) + shade2.Sample(MirrorSampler, sampleCoordinates2));
        highValue = (1.0 / 3.0) * (shade3.Sample(MirrorSampler, sampleCoordinates0) + shade3.Sample(MirrorSampler, sampleCoordinates1) + shade3.Sample(MirrorSampler, sampleCoordinates2));
        blendFactor = (luminance - 0.45f) / .15f;
    }
    else if (luminance < 0.75f)
    {
        lowValue = (1.0 / 3.0) * (shade3.Sample(MirrorSampler, sampleCoordinates0) + shade3.Sample(MirrorSampler, sampleCoordinates1) + shade3.Sample(MirrorSampler, sampleCoordinates2));
        highValue = (1.0 / 3.0) * (shade4.Sample(MirrorSampler, sampleCoordinates0) + shade4.Sample(MirrorSampler, sampleCoordinates1) + shade4.Sample(MirrorSampler, sampleCoordinates2));
        blendFactor = (luminance - 0.6f) / .15f;
    }
    else if (luminance < 0.9f)
    {
        lowValue = (1.0 / 3.0) * (shade4.Sample(MirrorSampler, sampleCoordinates0) + shade4.Sample(MirrorSampler, sampleCoordinates1) + shade4.Sample(MirrorSampler, sampleCoordinates2));
        highValue = (1.0 / 3.0) * (shade5.Sample(MirrorSampler, sampleCoordinates0) + shade5.Sample(MirrorSampler, sampleCoordinates1) + shade5.Sample(MirrorSampler, sampleCoordinates2));
        blendFactor = (luminance - 0.75f) / .15f;
    }
    else
    {
        lowValue = (1.0 / 3.0) * (shade5.Sample(MirrorSampler, sampleCoordinates0) + shade5.Sample(MirrorSampler, sampleCoordinates1) + shade5.Sample(MirrorSampler, sampleCoordinates2));
        highValue = 1.0f;
        blendFactor = (luminance - 0.9f) / .1f;
    }

    luminance = lerp(lowValue, highValue, blendFactor);
    return float4(luminance, luminance, luminance, luminance);
}
