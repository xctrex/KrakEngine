/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GBufferVisualizer.fx
Purpose: Shader for visualizing the GBuffer
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 2/9/2014
- End Header -----------------------------------------------------*/
#include "Common.fx"

Texture2D<float4> buffer       : register(t0);

SamplerState      pointSampler                  : register(s0);
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBufferPerFrame : register( b0 )
{
    matrix View;
    matrix Projection;
    float4 lightPosition;
    float2 ScreenSize;
};

cbuffer ConstantBufferPerObject : register( b1 )
{
    matrix World;
    float4 Animation;
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
    return buffer.Sample(pointSampler, input.TextureUV.xy);
}

float4 PS_Red(VS_OUTPUT input) : SV_TARGET
{
    return buffer.Sample(pointSampler, input.TextureUV.xy).rrrr;
}

float4 PS_Green(VS_OUTPUT input) : SV_TARGET
{
    return buffer.Sample(pointSampler, input.TextureUV.xy).gggg;
}

float4 PS_Blue(VS_OUTPUT input) : SV_TARGET
{
    return buffer.Sample(pointSampler, input.TextureUV.xy).bbbb;
}