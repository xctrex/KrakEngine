#pragma once
///////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh.hpp
//	3d traiagular mesh container which consists of a 
//	vertex buffer, index buffer, and a vertex declaration.
//	
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Vertex.h"

namespace KrakEngine
{
    class ChunkReader;

    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void * m_pVertexBufferData;
        UINT m_VertexSize;
        UINT m_VertexOffset;
        UINT * m_pIndexBufferData;
        UINT m_NumVertices;
        UINT m_NumPrimitives;
        UINT m_NumIndices;
        VertexType m_VertexType;

        void Initialize(const ComPtr<ID3D11Device1> &spD3DDevice1);
        void Set(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const;
        void Draw(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const;

        //void Initialize(IDirect3DDevice9 * device);
        //void SetAndDraw(IDirect3DDevice9 * device);

        ComPtr<ID3D11Buffer> m_spVertexBuffer;
        ComPtr<ID3D11Buffer> m_spIndexBuffer;
        ComPtr<ID3D11InputLayout> m_spVertexLayout;

        //IDirect3DVertexBuffer9 * d3dVertexBuffer;
        //IDirect3DIndexBuffer9 * d3dIndexBuffer;
        //IDirect3DVertexDeclaration9* vertexDecl;

        void ReadFrom(ChunkReader& reader);
    };
}
