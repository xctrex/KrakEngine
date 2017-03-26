#include "Common.fx"

Texture2D<float4> GradientBuffer       : register(t0);

SamplerState      PointSampler                  : register(s0);

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
void GetSurroundingPixels(in Texture2D<float4> buffer, in SamplerState pointSampler, in float2 texCoords, in float2 screenSize, out float4 pixels[3][3])
{
    float left, right, top, bottom;
    left = clamp(texCoords.x - (1.0f / screenSize.x), 0.0f, 1.0f);
    right = clamp(texCoords.x + (1.0f / screenSize.x), 0.0f, 1.0f);
    top = clamp(texCoords.y - (1.0f / screenSize.y), 0.0f, 1.0f);
    bottom = clamp(texCoords.y + (1.0f / screenSize.y), 0.0f, 1.0f);

    if (left < 0.0001f)
    {
        left = texCoords.x;
    }
    if (right > 0.9999f)
    {
        right = texCoords.x;
    }
    if (top < 0.0001f)
    {
        top = texCoords.y;
    }
    if (bottom > 0.9999f)
    {
        bottom = texCoords.y;
    }

    pixels[0][0] = buffer.Sample(pointSampler, float2(left, top));
    pixels[0][1] = buffer.Sample(pointSampler, float2(texCoords.x, top));
    pixels[0][2] = buffer.Sample(pointSampler, float2(right, top));
    pixels[1][0] = buffer.Sample(pointSampler, float2(left, texCoords.y));
    pixels[1][1] = buffer.Sample(pointSampler, float2(texCoords.x, texCoords.y));
    pixels[1][2] = buffer.Sample(pointSampler, float2(right, texCoords.y));
    pixels[2][0] = buffer.Sample(pointSampler, float2(left, bottom));
    pixels[2][1] = buffer.Sample(pointSampler, float2(texCoords.x, bottom));
    pixels[2][2] = buffer.Sample(pointSampler, float2(right, bottom));
}

float2 GradientDirection(float A, float B, float C, float D, float x, float E, float F, float G, float H)
{
    //|A||B||C|
    //|D||x||E|
    //|F||G||H|
    return (abs(A - x) * float2(-1.0f, 1.0f) + abs(B - x) * float2(0.0f, 1.0f) + abs(C - x) * float2(1.0f, 1.0f) + abs(D - x) * float2(-1.0f, 0.0f) + abs(E - x) * float2(1.0f, 0.0f) + abs(F - x) * float2(-1.0f, -1.0f) + abs(G - x) * float2(0.0f, -1.0f) + abs(H - x) * float2(1.0f, -1.0f)) / 8.0;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // Sample the surrounding pixels
    float4 pixels[3][3];
    GetSurroundingPixels(GradientBuffer, PointSampler, input.TextureUV, ScreenSize, pixels);

    // Calculate the gradient value at x
    float2 luminanceDirection = GradientDirection(pixels[0][0].x, pixels[0][1].x, pixels[0][2].x, pixels[1][0].x, pixels[1][1].x, pixels[1][2].x, pixels[2][0].x, pixels[2][1].x, pixels[2][2].x);

    // Store the result in the target buffer
    return float4(luminanceDirection.x, luminanceDirection.y,0.0f,0.0f);
}
