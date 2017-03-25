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
#include "CommonGBufferInput.fx"

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

static float2 Offsets[4] = {
    float2(-0.5f, 0.5f),
    float2( 0.5f, 0.5f),
    float2(-0.5f, -0.5f),
    float2( 0.5f, -0.5f),
};

static const float2 UV[6] = {
    float2(0.0f, 0.0f), // Top Left
    float2(1.0f, 0.0f), // Top Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(0.0f, 1.0f), // Bottom Left
    float2(0.0f, 0.0f), // Top Left
};

static const float4 Mask[4] = {
    float4(1.0f, 0.0f, 0.0f, 0.0f),
    float4(0.0f, 1.0f, 0.0f, 0.0f),
    float4(0.0f, 0.0f, 1.0f, 0.0f),
    float4(0.0f, 0.0f, 0.0f, 1.0f),
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
    /*VS_OUTPUT output;

    output.Pos = float4(BasePos[VertexID%6].xy * 0.5f + Offsets[VertexID / 6], 0.0f, 1.0f);
    output.TextureUV = UV[VertexID % 6].xy;
    output.SampleMask = Mask[VertexID / 6].xyzw;

    return output;*/
    
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
    
    // The code in this pixel shader uses a mask to mimick the following pseudocode without
    // using conditionals, which are expensive in hlsl
    // if (pixel is in the rect for displaying depth)
    //     finalColor = depth
    // else if (pixel is in the rect for displaying color)
    //     finalColor = color
    // else if (pixel is in the rect for displaying normals
    //     finalColor = Normal
    // else
    //     finalColor = Specular values

    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    // If the pixel is in the rect for displaying depth (as determined in the vertex shader), SampleMask.x will equal 1 here and SampleMask.yzw will all equal 0.
    // Thus, only the depth will be added to finalColor, and all other values (color, normal, specular values) will be zero after being multiplied by the mask.
    // If the pixel is in the rect for displaying color, only SampleMask.y will equal 1 and thus only color data will be added to final color.
    // The same logic goes for displaying normal and specular intensity data.
    //finalColor += float4(1.0f - saturate(GBufferData.LinearDepth / 75.0f), 1.0f - saturate(GBufferData.LinearDepth / 125.0f), 1.0f - saturate(GBufferData.LinearDepth / 200.0f), 0.0f);// * input.SampleMask.xxxx;
    //finalColor += float4(GBufferData.Color.xyz, 0.0f);// * input.SampleMask.yyyy;
    //finalColor += float4(GBufferData.Normal.xyz * 0.5f + 0.5f, 0.0f);// * input.SampleMask.zzzz;
    //finalColor += float4(GBufferData.SpecularIntensity, GBufferData.SpecularExponent, 0.0f, 0.0f);// * input.SampleMask.wwww;
    float2 start = get2dPoint(GBufferData.Position, View, Projection, ScreenSize.x, ScreenSize.y);
    float2 end = get2dPoint(GBufferData.Position + GBufferData.Normal, View, Projection, ScreenSize.x, ScreenSize.y);
    float2 xyprojection = end - start;
    xyprojection /= sqrt(xyprojection.x * xyprojection.x + xyprojection.y * xyprojection.y);
    finalColor.rg = xyprojection;
    //finalColor.rgb = GBufferData.Normal.xyz;
    return finalColor;
}