/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: DirectionalLight.fx
Purpose: Shader for rendering directional lights
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 2/9/2014
- End Header -----------------------------------------------------*/

//--------------------------------------------------------------------------------------
// GBuffer Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D<float>  DepthTexture                  : register( t0 );
Texture2D<float4> ColorSpecularIntensityTexture : register( t1 );
Texture2D<float3> NormalTexture                 : register( t2 );
//Texture2D<float4> SpecularExponentTexture       : register( t3 );
SamplerState      PointSampler                  : register( s0 );

cbuffer ConstantBufferGBufferUnpack : register( b2 )
{
    float4 PerspectiveValues : packoffset( c0 );
    float4x4 ViewInverse     : packoffset( c1 );
};

struct SurfaceData
{
    float LinearDepth;
    float3 Color;
    float3 Normal;
    float SpecularIntensity;
//    float SpecularExponent;
};

float3 DecodeNormal(float2 EncodedNormal)
{
    float4 DecodedNormal = EncodedNormal.xyyy * float4(2.0f, 2.0f, 0.0f, 0.0f) + float4(-1.0f, -1.0f, 1.0f, -1.0f);
    DecodedNormal.z = dot(DecodedNormal.xyz, -DecodedNormal.xyw);
    DecodedNormal.xy *= sqrt(DecodedNormal.z);
    return DecodedNormal.xyz * 2.0f + float3(0.0f, 0.0f, -1.0f);
}

float ConvertZToLinearDepth(float depth)
{
    return PerspectiveValues.z / (depth + PerspectiveValues.w);
}

SurfaceData UnpackGBuffer(float2 UV)
{
    SurfaceData output;

    // Get the depth from the GBuffer and convert it to linear space
    float depth = DepthTexture.Sample( PointSampler, UV.xy ).x;
    output.LinearDepth = ConvertZToLinearDepth(depth);

    // Get the Color and specular intensity from the GBuffer
    float4 ColorSpecularIntensity = ColorSpecularIntensityTexture.Sample( PointSampler, UV.xy );
    output.Color = ColorSpecularIntensity.xyz;
    output.SpecularIntensity = ColorSpecularIntensity.w;

    // Get the normal and unpack it
    output.Normal = NormalTexture.Sample( PointSampler, UV.xy ).xyz;
    output.Normal = normalize(output.Normal * 2.0f - 1.0f);
    
    // Get the specular exponent and scale it back to the original range
//    output.SpecularExponent = SpecularExponentTexture.Sample( PointSampler, UV.xy ).x;

    return output;
}

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

static const float2 BasePos[6] = {
    float2(-1.0f,  1.0f),
    float2( 1.0f,  1.0f),
    float2( 1.0f, -1.0f),
    float2( 1.0f, -1.0f),
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
    SurfaceData GBufferData = UnpackGBuffer(input.TextureUV.xy);
    
    float4 finalColor = float4(GBufferData.Color.xyz, 0.0f);

    return finalColor;
}