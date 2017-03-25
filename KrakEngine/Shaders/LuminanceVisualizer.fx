#include "Common.fx"
#include "CommonGBufferInput.fx"

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

    //stroke direction
    float2 start = get2dPoint(GBufferDataX.Position, View, Projection, ScreenSize.x, ScreenSize.y);
    float2 end = get2dPoint(GBufferDataX.Position + GBufferDataX.Normal, View, Projection, ScreenSize.x, ScreenSize.y);
    float2 xyprojection = end - start;
    xyprojection /= sqrt(xyprojection.x * xyprojection.x + xyprojection.y * xyprojection.y);
    float2 direction = xyprojection;

    if (input.TextureUV.x < 0.5f)
    {
        direction = float2(1.0f, 1.0f);
    }
    else
    {
        direction = float2(1.0f, -1.0f);
    }
    // Store the gradient value in the LuminanceBuffer
    return float4(luminance, direction.x, direction.y, luminance);
}
