/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Vertex.h
Purpose: Definition of Vertex Types
Language: C++, MSC
Platform: Windows 7
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 1/20/2014
- End Header -----------------------------------------------------*/
#pragma once
#include "Precompiled.h"

namespace KrakEngine
{
    enum VertexType
    {
        VertexTypeFBXBinModel,
        VertexTypeFBXBinSkinnedModel,
        VertexTypeAlbedoModel,
        VertexTypeTexturedModel
    };

    struct FBXBinModelVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Norm;
        XMFLOAT2 Tex;
    };

    struct FBXBinSkinnedModelVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Norm;
        XMFLOAT2 Tex;
        float w[4];
        byte i[4];
    };

    struct Vertex
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT4 Color;
    };
    
	struct AlbedoModelVertex
	{
		XMFLOAT3 Pos;   // position
        XMFLOAT3 Normal;// normal
		XMFLOAT4 Albedo; // color
	};
    
    struct AlbedoModel
    {
        AlbedoModelVertex* VertexArray;
        int NumVertices;
    };

    struct TexturedModelVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 TextureUV;
    };

    struct TexturedModel
    {
        TexturedModelVertex* VertexArray;
        int NumVertices;
    };

	struct SimpleVertex
	{
		XMFLOAT3 Pos;
	};

    struct SpriteVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
        XMFLOAT2 TextureUV;
    };

    struct FullScreenQuadVertex
    {
        XMFLOAT4 Pos;
        XMFLOAT2 TextureUV;
    };

	struct ConstantBufferPerObjectVS
	{
		XMFLOAT4X4 World;
	};

    struct ConstantBufferPerObjectPS
    {
        float SpecularExponent;
        float SpecularIntensity;
    };

    struct ConstantBufferPerFrame
	{
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
        XMFLOAT4 LightPosition;
        XMFLOAT2 ScreenSize;
        float StrokeRotation;
	};

    struct ConstantBufferPerSpriteObject
    {
		XMFLOAT4X4 World;
        XMFLOAT4 Animation;
    };

    struct ConstantBufferGBufferUnpack
    {
        XMFLOAT4 PerspectiveValues;
        XMFLOAT4X4 InverseViewTransform;
    };
}
