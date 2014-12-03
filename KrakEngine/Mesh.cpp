///////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Mesh.h"
#include "File\ChunkReader.hpp"
#include "File\FileElements.hpp"

namespace KrakEngine
{
    Mesh::Mesh()
    {
        m_pVertexBufferData = NULL;
        m_pIndexBufferData = NULL;
        m_spVertexBuffer = nullptr;
        m_spIndexBuffer = nullptr;
        //vertexDecl = NULL;
        m_NumVertices = 0;
        m_NumPrimitives = 0;
        m_VertexOffset = 0;
    }

    Mesh::~Mesh()
    {
        //SafeRelease(vertexDecl);
        /*m_spVertexBuffer.ReleaseAndGetAddressOf();
        m_spVertexBuffer = nullptr;
        m_spVertexLayout.ReleaseAndGetAddressOf();
        m_spVertexLayout = nullptr;
        m_spIndexBuffer.ReleaseAndGetAddressOf();
        m_spIndexBuffer = nullptr;
        SafeDelete(m_pVertexBufferData);
        SafeDelete(m_pIndexBufferData);*/
    }


    void Mesh::ReadFrom(ChunkReader& file)
    {
        GChunk meshChunk = file.ReadChunkHeader();

        file.Read(m_VertexType);
        switch (m_VertexType)
        {
        case VertexTypeFBXBinModel:
            m_VertexSize = sizeof(FBXBinModelVertex);
            break;
        case VertexTypeFBXBinSkinnedModel:
            m_VertexSize = sizeof(FBXBinSkinnedModelVertex);
            break;
        default:
            ThrowErrorIf(true, "VertexType unrecognized");
            break;
        }
        // VertexDescription::Desc[VertexType].SizeOfVertex;

        file.Read(m_NumIndices);
        m_pIndexBufferData = new UINT[m_NumIndices];
        file.ReadArray(m_pIndexBufferData, m_NumIndices);

        file.Read(m_NumVertices);
        m_pVertexBufferData = new byte[m_NumVertices * m_VertexSize];
        file.ReadArraySize(m_pVertexBufferData, m_NumVertices * m_VertexSize);

        m_NumPrimitives = m_NumIndices / 3;
    }

    void Mesh::Initialize(const ComPtr<ID3D11Device1> &spD3DDevice1)
    {
        //Load into vertex Buffer
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = m_VertexSize * m_NumVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        // Initialize the data in the buffer
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = m_pVertexBufferData;

        DXThrowIfFailed(
            spD3DDevice1->CreateBuffer(&bd, &InitData, m_spVertexBuffer.GetAddressOf()));
        // TODO: Write only?
        //device->CreateVertexBuffer(NumVertices*VertexSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &d3dVertexBuffer, NULL);

        // Create index buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(UINT)* m_NumIndices;        // 36 vertices needed for 12 triangles in a triangle list
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        InitData.pSysMem = m_pIndexBufferData;
        //InitData.pSysMem = indices;
        DXThrowIfFailed(
            spD3DDevice1->CreateBuffer(&bd, &InitData, m_spIndexBuffer.ReleaseAndGetAddressOf()));
    }

    void Mesh::Set(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const
    {
        // Set vertex buffer
        spD3DDeviceContext1->IASetVertexBuffers(0, 1, m_spVertexBuffer.GetAddressOf(), &m_VertexSize, &m_VertexOffset);

        // Set index buffer
        spD3DDeviceContext1->IASetIndexBuffer(m_spIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // Set primitive topology
        spD3DDeviceContext1->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void Mesh::Draw(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const
    {
        spD3DDeviceContext1->DrawIndexed(m_NumIndices, 0, 0);
    }
}
