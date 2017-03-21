#include "Common.fx"

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


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 TextureUV : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos        : SV_POSITION;
    float2 TextureUV  : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    output.Pos = input.Pos;
    output.TextureUV = input.TextureUV;

    return output;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // Upack the GBuffer of this pixel (x)

    SurfaceData GBufferDataX = UnpackGBuffer(input.TextureUV.xy);
    float3 lightDirection = normalize(lightPosition.xyz - GBufferDataX.Position);
    float lightIntensity = dot(GBufferDataX.Normal, lightDirection);
    float luminance = lightIntensity * (GBufferDataX.Color.r + GBufferDataX.Color.g + GBufferDataX.Color.b) / 3.0f;
    float2 xy = input.TextureUV.xy * ScreenSize / float2(143.0f, 143.0f);
    float2 sampleCoordinates = float2(xy.x*cos(StrokeRotation) - xy.y * sin(StrokeRotation), xy.x * sin(StrokeRotation) + xy.y * cos(StrokeRotation));
    float lowValue = 0.0f;
    float highValue = 1.0f;
    float blendFactor = 0.5f;
    if (luminance < 0.15f)
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

    luminance = lerp(lowValue, highValue, blendFactor);
    //return shade0.Sample(PointSampler, input.TextureUV.xy * ScreenSize / float2(143.0f, 143.0f));
    //return lightPosition;
    //return GBufferDataX.Position;
    // Store the gradient value in the LuminanceBuffer
    return float4(luminance, luminance, luminance, luminance);
}
