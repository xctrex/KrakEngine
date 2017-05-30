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
void GetSurroundingPixels3x3(in Texture2D<float4> buffer, in SamplerState pointSampler, in float2 texCoords, in float2 screenSize, out float4 pixels[3][3])
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

void GetSurroundingPixels5x5(in Texture2D<float4> buffer, in SamplerState pointSampler, in float2 texCoords, in float2 screenSize, out float4 pixels[5][5])
{
    float left, right, top, bottom, midLeft, midRight, midTop, midBottom;

    midLeft = clamp(texCoords.x - (1.0f / screenSize.x), 0.0f, 1.0f);
    midRight = clamp(texCoords.x + (1.0f / screenSize.x), 0.0f, 1.0f);
    midTop = clamp(texCoords.y - (1.0f / screenSize.y), 0.0f, 1.0f);
    midBottom = clamp(texCoords.y + (1.0f / screenSize.y), 0.0f, 1.0f);

    left = clamp(texCoords.x - (2.0f / screenSize.x), 0.0f, 1.0f);
    right = clamp(texCoords.x + (2.0f / screenSize.x), 0.0f, 1.0f);
    top = clamp(texCoords.y - (2.0f / screenSize.y), 0.0f, 1.0f);
    bottom = clamp(texCoords.y + (2.0f / screenSize.y), 0.0f, 1.0f);

    // Clamp mid texcoords to edge of screen
    if (midLeft < 0.0001f)
    {
        midLeft = texCoords.x;
    }
    if (midRight > 0.9999f)
    {
        midRight = texCoords.x;
    }
    if (midTop < 0.0001f)
    {
        midTop = texCoords.y;
    }
    if (midBottom > 0.9999f)
    {
        midBottom = texCoords.y;
    }

    // Clamp far texcoords to edge of screen
    if (left < 0.0001f)
    {
        left = midLeft;
    }
    if (right > 0.9999f)
    {
        right = midRight;
    }
    if (top < 0.0001f)
    {
        top = midTop;
    }
    if (bottom > 0.9999f)
    {
        bottom = midBottom;
    }
    // Top row
    pixels[0][0] = buffer.Sample(pointSampler, float2(left, top));
    pixels[0][1] = buffer.Sample(pointSampler, float2(midLeft, top));
    pixels[0][2] = buffer.Sample(pointSampler, float2(texCoords.x, top));
    pixels[0][3] = buffer.Sample(pointSampler, float2(midRight, top));
    pixels[0][4] = buffer.Sample(pointSampler, float2(right, top));
    // Second row
    pixels[1][0] = buffer.Sample(pointSampler, float2(left, midTop));
    pixels[1][1] = buffer.Sample(pointSampler, float2(midLeft, midTop));
    pixels[1][2] = buffer.Sample(pointSampler, float2(texCoords.x, midTop));
    pixels[1][3] = buffer.Sample(pointSampler, float2(midRight, midTop));
    pixels[1][4] = buffer.Sample(pointSampler, float2(right, midTop));
    // Third row
    pixels[2][0] = buffer.Sample(pointSampler, float2(left, texCoords.y));
    pixels[2][1] = buffer.Sample(pointSampler, float2(midLeft, texCoords.y));
    pixels[2][2] = buffer.Sample(pointSampler, float2(texCoords.x, texCoords.y));
    pixels[2][3] = buffer.Sample(pointSampler, float2(midRight, texCoords.y));
    pixels[2][4] = buffer.Sample(pointSampler, float2(right, texCoords.y));
    // Fourth row
    pixels[3][0] = buffer.Sample(pointSampler, float2(left, midBottom));
    pixels[3][1] = buffer.Sample(pointSampler, float2(midLeft, midBottom));
    pixels[3][2] = buffer.Sample(pointSampler, float2(texCoords.x, midBottom));
    pixels[3][3] = buffer.Sample(pointSampler, float2(midRight, midBottom));
    pixels[3][4] = buffer.Sample(pointSampler, float2(right, midBottom));
    // Fifth row
    pixels[4][0] = buffer.Sample(pointSampler, float2(left, bottom));
    pixels[4][1] = buffer.Sample(pointSampler, float2(midLeft, bottom));
    pixels[4][2] = buffer.Sample(pointSampler, float2(texCoords.x, bottom));
    pixels[4][3] = buffer.Sample(pointSampler, float2(midRight, bottom));
    pixels[4][4] = buffer.Sample(pointSampler, float2(right, bottom));
}

float2 ConvolveRed3x3(in float4 pixels[3][3], in float convolutionFilter[3][3], in float denominator)
{
    float result = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result += abs(pixels[i][j].r - pixels[1][1].r) * convolutionFilter[i][j];
        }
    }
    return result /= denominator;
}

float2 ConvolveGreen3x3(in float4 pixels[3][3], in float convolutionFilter[3][3], in float denominator)
{
    float result = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result += abs(pixels[i][j].g - pixels[1][1].g) * convolutionFilter[i][j];
        }
    }
    return result /= denominator;
}

float2 ConvolveRed5x5(in float4 pixels[5][5], in float convolutionFilter[5][5], in float denominator)
{
    float result = 0.0f;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            result += abs(pixels[i][j].r - pixels[2][2].r) * convolutionFilter[i][j];
        }
    }
    return result /= denominator;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Convolve7x7(float2 centerUV, float2 offsets[3], float weights[3], Texture2D inputTexture, SamplerState textureSampler, float2 textureSize)
{
    ////////////////////////////////////////////////
    float4 colorOut = { 0, 0, 0, 0 };
    const int stepCount = 3;

    colorOut = inputTexture.Sample(textureSampler, centerUV) * weights[0];
    for (int i = 1; i < stepCount; i++)
    {
        float2 texCoordOffset = offsets[i] / textureSize;
        float4 color = inputTexture.Sample(textureSampler, centerUV + texCoordOffset) +
            inputTexture.Sample(textureSampler, centerUV - texCoordOffset);

        colorOut += weights[i] * color;
    }

    return colorOut;
}

float4 Convolve7x7_PerChannelWeight(float2 centerUV, float2 offsets[3], float4 weights[3], Texture2D inputTexture, SamplerState textureSampler, float2 textureSize)
{
    ////////////////////////////////////////////////
    float4 colorOut = { 0, 0, 0, 0 };
    const int stepCount = 3;

    colorOut = inputTexture.Sample(textureSampler, centerUV) * weights[0];
    for (int i = 1; i < stepCount; i++)
    {
        float2 texCoordOffset = offsets[i] / textureSize;
        float4 color = inputTexture.Sample(textureSampler, centerUV + texCoordOffset) +
            inputTexture.Sample(textureSampler, centerUV - texCoordOffset);

        colorOut += weights[i] * color;
    }

    return colorOut;
}

float4 VerticalSobel_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 centerUV = input.TextureUV * 4.0;
    ////////////////////////////////////////////////;
    // Kernel width 7 x 7
    const float2 offsets[3] = { {0.0, 0.0}, {1.3846153846, 0.0}, {3.2307692308, 0.0} };
    //const float weights[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };
    const float4 weights[3] = { {2.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 0.0, 0.0}, {1.0, -1.0, 0.0, 0.0} };
    ////////////////////////////////////////////////
    return Convolve7x7_PerChannelWeight(centerUV, offsets, weights, GradientBuffer, PointSampler, ScreenSize / 4.0);
}

float4 HorizontalSobel_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 centerUV = input.TextureUV* 4.0;
    ////////////////////////////////////////////////;
    // Kernel width 7 x 7
    const float2 offsets[3] = { {0.0, 0.0}, {1.3846153846, 0.0}, {3.2307692308, 0.0} };
    //const float weights[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };
    const float4 weights[3] = { {0.0, 2.0, 0.0, 0.0 }, {1.0, 1.0, 0.0, 0.0}, {-1.0, 1.0, 0.0, 0.0} };
    ////////////////////////////////////////////////
    return Convolve7x7_PerChannelWeight(centerUV, offsets, weights, GradientBuffer, PointSampler, ScreenSize / 4.0);
}

float4 GaussianBlurHorizontal_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 centerUV = input.TextureUV* 4.0;
    const float2 offsets[3] = { { 0.0, 0.0 },{ 1.3846153846, 0.0 },{ 3.2307692308, 0.0 } };
    const float weights[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };
    return Convolve7x7(centerUV, offsets, weights, GradientBuffer, PointSampler, ScreenSize / 4.0);
}

float4 GaussianBlurVertical_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 centerUV = input.TextureUV* 4.0;
    const float2 offsets[3] = { { 0.0, 0.0 },{ 0.0, 1.3846153846 },{ 0.0, 3.2307692308 } };
    const float weights[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };
    return Convolve7x7(centerUV, offsets, weights, GradientBuffer, PointSampler, ScreenSize / 4.0);
}

