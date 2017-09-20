#include "Common.fx"


Texture2D<float4> GradientBuffer      : register(t0);

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
    float4 Direction0 : TEXCOORD1;
    float4 Direction1  : TEXCOORD2;
    float4 Direction2 : TEXCOORD3;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT LuminanceStrokeDirection_VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = float4(input.Pos, 1.0f);
    output.Pos = mul(output.Pos, World);
    output.WorldPos = output.Pos.xyz / output.Pos.w;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Normal = mul(float4(input.Normal, 0.0f), World).xyz;

    // position to vertex clip-space
    float4 fake_frag_coord = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 projectionSpaceCoord = output.Pos;//Projection * View * World * input.Pos;
    //Is clipped? After that, you need to clip the vector. If any of the vector's 3 first components is greater in absolute value than the fourth component, then it is outside the field of view of the camera and should be clipped.
    float4 imageSpaceCoord = float4(projectionSpaceCoord.x / projectionSpaceCoord.w, projectionSpaceCoord.y / projectionSpaceCoord.w, projectionSpaceCoord.z / projectionSpaceCoord.w, 1.0f);
    fake_frag_coord.x = ((imageSpaceCoord.x + 1.0f) / 2.0f);
    fake_frag_coord.y = ((imageSpaceCoord.y + 1.0f) / 2.0f);
    
    // Initialize all values to 0, this way each vertex is interpolated between 0 and the desired direction value, which is okay as long as the direction's x and y are interpolated equally
    output.Direction0 = output.Direction1 = output.Direction2 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (input.id % 3 == 0)
    {
        output.Direction0 = GradientBuffer.SampleLevel(PointSampler, fake_frag_coord, 0);
    }
    else if (input.id % 3 == 1)
    {
        output.Direction1 = GradientBuffer.SampleLevel(PointSampler, fake_frag_coord, 0);
    }
    else// if(input.id %3 == 2)
    {
        output.Direction2 = GradientBuffer.SampleLevel(PointSampler, fake_frag_coord, 0);
    }
    return output;
};

float2 GetRotationFromDirection(float2 direction)
{
    if (direction.x < FLT_EPSILON && direction.y < FLT_EPSILON && direction.x >= 0.0 && direction.y >= 0.0)
    {
        // Force a direciton if none was set in the gradient buffer
        direction.x = 0.0;
        direction.y = 1.0; // straight up and down so it's somewhat clear when debugging
    }

    direction.xy = normalize(direction);
    
    return (atan2(direction.y, direction.x) + PI) / (2.0 * PI);
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 LuminanceStrokeDirection_PS(VS_OUTPUT input) : SV_TARGET
{
    float2 screenSpacePosition = input.Pos.xy;
    screenSpacePosition /= ScreenSize;

    float4 directionsOut = { 1.0, 1.0, 1.0, 1.0 };
    directionsOut.r = GetRotationFromDirection(input.Direction0.xy);
    directionsOut.g = GetRotationFromDirection(input.Direction1.xy);
    directionsOut.b = GetRotationFromDirection(input.Direction2.xy);
    
    return directionsOut;
}
