/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GBufferShader.fx
Purpose: Shader for packing the GBuffer
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 2/9/2014
- End Header -----------------------------------------------------*/

//--------------------------------------------------------------------------------------
// Vertex Shader Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBufferPerFrameVS : register(b0)
{
    matrix View;
    matrix Projection;
    float2 ScreenSize;
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
    float4 Color : COLOR0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL0;
    float3 WorldPos : POSITION;
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = float4(input.Pos, 1.0f);
    output.Pos = mul(output.Pos, World);
    output.WorldPos = output.Pos.xyz / output.Pos.w;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = input.Color;
    output.Normal = mul(float4(input.Normal, 0.0f), World).xyz;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBufferPerObjectPS : register(b0)
{
    float SpecularExponent  : packoffset(c0);
    float SpecularIntensity : packoffset(c0.y);
};

//--------------------------------------------------------------------------------------
struct PS_GBUFFER_OUT
{
    float4 ColorSpecularIntensity : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    //float4 SpecularExponent : SV_TARGET2;
};

static const float2 g_SpecularExponentRange = { 0.1f, 250.0f };

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_GBUFFER_OUT PackGBuffer(float3 Color, float3 Normal, float SpecularIntensity, float SpecularExponent)
{
    PS_GBUFFER_OUT output;

    // Normalize the specular power
    float NormalizedSpecularExponent = (SpecularExponent - g_SpecularExponentRange.x) / g_SpecularExponentRange.y;

    // Pack all the data into the GBuffer structure
    output.ColorSpecularIntensity = float4(Color.rgb, SpecularIntensity);
    output.Normal = float4(Normal * 0.5f + 0.5f, 0.0f);
    //output.SpecularExponent = float4(SpecularExponent, 0.0f, 0.0f, 0.0f);

    return output;
}

PS_GBUFFER_OUT PS(VS_OUTPUT input)
{
    return PackGBuffer(input.Color.rgb, normalize(input.Normal), SpecularIntensity, SpecularExponent);
}
