/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Model.cpp
Purpose: Component for Model
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton
Creation date: 1/20/2014
- End Header -----------------------------------------------------*/

#include "Model.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "Factory.h"
#include "RigidBody.h"

#include "File\ChunkReader.hpp"
#include "File\FileElements.hpp"
#include "DrawingState.h"

namespace KrakEngine{
    Model::Model() : 
        Component(ModelComp),
        m_NumIndices(36),
        m_Stride(28),
        m_Offset(0),
        m_Size(1.0f, 1.0f, 1.0f),
        m_SpecularExponent(1.0f),
        m_SpecularIntensity(1.0f),
        m_ModelName("BoxModel"),
        m_VertexType(VertexTypeFBXBinModel),
		m_IsSelected(false),
        m_TextureName("Default"),
        m_Mesh(NULL),
        m_Controller(NULL),
        m_OffsetPosition(0.f, 0.f, 0.f),
        m_OffsetRotation(0.f, 0.f, 0.f)
    {
        g_GRAPHICSSYSTEM->AddModelToList(this);
    };
    
    void Model::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, m_Size.x, "SizeX");
        InitializeDataMember(stream, m_Size.y, "SizeY");
        InitializeDataMember(stream, m_Size.z, "SizeZ");
        InitializeDataMember(stream, m_ModelName, "ModelName");
        InitializeDataMember(stream, m_VertexType, "VertexType");
        InitializeDataMember(stream, m_TextureName, "TextureName");
        InitializeDataMember(stream, m_OffsetPosition.x, "OffsetPosX");
        InitializeDataMember(stream, m_OffsetPosition.y, "OffsetPosY");
        InitializeDataMember(stream, m_OffsetPosition.z, "OffsetPosZ");
        InitializeDataMember(stream, m_OffsetRotation.x, "OffsetRotX");
        InitializeDataMember(stream, m_OffsetRotation.y, "OffsetRotY");
        InitializeDataMember(stream, m_OffsetRotation.z, "OffsetRotZ");

        m_OffsetRotation.x = XMConvertToRadians(m_OffsetRotation.x);
        m_OffsetRotation.y = XMConvertToRadians(m_OffsetRotation.y);
        m_OffsetRotation.z = XMConvertToRadians(m_OffsetRotation.z);
        
        LoadBinaryModel("Assets\\Models\\" + m_ModelName + ".bin", g_GRAPHICSSYSTEM->GetD3DDevice1());
        g_GRAPHICSSYSTEM->LoadTexture(m_TextureName, "Assets\\Models\\" + m_TextureName + ".DDS");


        // Set the vertex layout and shaders
        m_spVertexLayout = g_GRAPHICSSYSTEM->GetVertexLayout(m_VertexType);
        m_spVertexShader = g_GRAPHICSSYSTEM->GetGBufferVertexShader(m_VertexType);
        m_spPixelShader = g_GRAPHICSSYSTEM->GetGBufferPixelShader(m_VertexType);

        // Create the vertex buffer
        CreateBuffers(g_GRAPHICSSYSTEM->GetD3DDevice1());
    }

	void Model::SerializeToXML(tinyxml2::XMLElement * element){

		WriteDataMember(element, m_Size.x, "SizeX");
		WriteDataMember(element, m_Size.y, "SizeY");
		WriteDataMember(element, m_Size.z, "SizeZ");
        WriteDataMember(element, m_ModelName, "ModelName");
        WriteDataMember<VertexType>(element, m_VertexType, "VertexType");
        WriteDataMember(element, m_TextureName, "TextureName");
	}

    void Model::Initialize(){}

    void Model::Shutdown(){}

    void Model::CreateBuffers(const ComPtr<ID3D11Device1> &spD3DDevice1){
        
		D3D11_BUFFER_DESC bd;
		D3D11_SUBRESOURCE_DATA InitData;

        switch(m_VertexType)
        {
        case VertexTypeAlbedoModel:
        {
            // Create the buffer description
            const AlbedoModel* model = g_GRAPHICSSYSTEM->GetAlbedoModel(m_ModelName);
		    ZeroMemory(&bd, sizeof(bd));
		    bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(AlbedoModelVertex) * model->NumVertices;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		    bd.CPUAccessFlags = 0;

            // Initialize the data in the buffer
		    ZeroMemory(&InitData, sizeof(InitData));
            InitData.pSysMem = model->VertexArray;
		    DXThrowIfFailed(
			    spD3DDevice1->CreateBuffer(
				    &bd, 
				    &InitData, 
                    m_spVertexBuffer.GetAddressOf()));

		    // Set vertex buffer
            m_Stride = sizeof(AlbedoModelVertex);
		    m_Offset = 0;
            break;
        }
        case VertexTypeTexturedModel:
        {
            // Create the buffer description
            const TexturedModel* model = g_GRAPHICSSYSTEM->GetTexturedModel(m_ModelName);
		    ZeroMemory(&bd, sizeof(bd));
		    bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(TexturedModelVertex) * model->NumVertices;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		    bd.CPUAccessFlags = 0;

            // Initialize the data in the buffer
		    ZeroMemory(&InitData, sizeof(InitData));
            InitData.pSysMem = model->VertexArray;
		    DXThrowIfFailed(
			    spD3DDevice1->CreateBuffer(
				    &bd, 
				    &InitData, 
                    m_spVertexBuffer.GetAddressOf()));

		    // Set vertex buffer
            m_Stride = sizeof(TexturedModelVertex);
		    m_Offset = 0;
            break;
        }
        case VertexTypeFBXBinModel :
            m_Stride = sizeof(FBXBinModelVertex);
            m_Offset = 0;
            return;
        default:
            ThrowErrorIf(true, "VertexType not found");
            break;
        }
       
		// Create index buffer
		WORD indices[] =
		{
            // Front
            0, 1, 2,
            3, 1, 0,
            
            // Back
            4, 5, 6,
            5, 7, 6,    

            // Top
            9,  8, 10,
            10, 8, 11,

            // Bottom
            15, 13, 14,
            13, 15, 12,

            // Left
            17, 18, 19,
            18, 17, 16,

            // Right
            22, 20, 21,
            23, 22, 21,
		};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD)* ARRAYSIZE(indices);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
        
        // Set the number of indices
        m_NumIndices = ARRAYSIZE(indices);

		DXThrowIfFailed(
			spD3DDevice1->CreateBuffer(&bd, &InitData, m_spIndexBuffer.ReleaseAndGetAddressOf()));
    }
       
    void Model::Update(float dt){
        if(GetOwner()){
            Transform* t = GetOwner()->has(Transform);

            XMStoreFloat4x4(&m_World, 
                XMMatrixScaling(m_Size.x, m_Size.y, m_Size.z) *
                XMMatrixRotationX(XMConvertToRadians(t->GetRotation().x) + m_OffsetRotation.x) * 
                XMMatrixRotationY(XMConvertToRadians(t->GetRotation().y) + m_OffsetRotation.y) * 
                XMMatrixRotationZ(XMConvertToRadians(t->GetRotation().z) + m_OffsetRotation.z) *
                XMMatrixTranslation(t->GetPosition().x + m_OffsetPosition.x, t->GetPosition().y + m_OffsetPosition.y, t->GetPosition().z + m_OffsetPosition.z));
        }
        
        if (m_Controller)
        {
            m_Controller->Update(dt);

            //if (g_GRAPHICSSYSTEM->IsDrawBindPose()){
                m_Controller->ProcessBindPose();
            //}
            //else{
            //    m_Controller->Process();
            //}

        }
    }

    void Model::Skin(const ComPtr<ID3D11Device1> &spD3DDevice1, const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1)
    {
        if (m_Controller)
        {
            std::vector< XMFLOAT4X4 > boneMatrices;
            boneMatrices.resize(m_Controller->m_pSkeleton->m_Bones.size());
            for (UINT i = 0; i < boneMatrices.size(); ++i)
            {
                //The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
                XMStoreFloat4x4(&boneMatrices[i], XMMatrixMultiply(XMLoadFloat4x4(&m_Controller->m_pSkeleton->GetModelToBoneSpaceTransform(i)), XMLoadFloat4x4(&m_Controller->m_BoneMatrixBuffer[i])));
            }

            //Software skinning used to debug problems in the pipeline
            FBXBinSkinnedModelVertex *pDest = (FBXBinSkinnedModelVertex *)m_Mesh->m_pSkinnedVertexBufferData;
            FBXBinSkinnedModelVertex *pSource = (FBXBinSkinnedModelVertex *)m_Mesh->m_pVertexBufferData;

            for (UINT i = 0; i < m_Mesh->m_NumVertices; ++i)
            {
                XMFLOAT3 v = XMFLOAT3(0.f, 0.f, 0.f);
                XMFLOAT3 n = XMFLOAT3(0.f, 0.f, 0.f);
                for (int w = 0; w < 4; ++w)
                {
                    XMFLOAT3 tn, tp;
                    UINT boneIndex = pSource[i].i[w];
                    XMFLOAT4X4 boneMatrix = boneMatrices[boneIndex];
                    ////(x,y,z,1) Homogeneous transform
                    XMStoreFloat3(&tp, XMVector3TransformCoord(XMLoadFloat3(&pSource[i].Pos), XMLoadFloat4x4(&boneMatrix)));
                    //(x,y,z,0) Normal transform remove translation(technically the matrix should be MatrixTranspose(MatrixInverse(tn)) but
                    //since we are using only uniform scaling this is equivalent)
                    XMStoreFloat3(&tn, XMVector3TransformNormal(XMLoadFloat3(&pSource[i].Norm), XMLoadFloat4x4(&boneMatrix)));

                    XMStoreFloat3(&tn, XMVectorScale(XMLoadFloat3(&tn), pSource[i].w[w]));
                    XMStoreFloat3(&tp, XMVectorScale(XMLoadFloat3(&tp), pSource[i].w[w]));
                    XMStoreFloat3(&v, XMVectorAdd(XMLoadFloat3(&v), XMLoadFloat3(&tp)));
                    XMStoreFloat3(&n, XMVectorAdd(XMLoadFloat3(&n), XMLoadFloat3(&tn)));
                }
                pDest[i].Pos = v;
                pDest[i].Norm = n;
                pDest[i].Tex = pSource[i].Tex;
            }

            spD3DDeviceContext1->UpdateSubresource(m_Mesh->m_spSkinnedVertexBuffer.Get(), 0, nullptr, m_Mesh->m_pSkinnedVertexBufferData, 0, 0);
        }
    }

    void Model::Draw(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectVS, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectPS)const{

        ShaderResources resources(
        {},
        {},
        { g_GRAPHICSSYSTEM->GetTexture(m_TextureName).Get() },
        { g_GRAPHICSSYSTEM->GetSampler().Get() });

        DrawShader(spD3DDeviceContext1, spConstantBufferPerObjectVS, spConstantBufferPerObjectPS, m_spVertexShader, m_spPixelShader, resources);
    }

    void Model::DrawShader(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectVS, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectPS,
        const ComPtr<ID3D11VertexShader> &spVertexShader, const ComPtr<ID3D11PixelShader> &spPixelShader, ShaderResources &resources)const {
        if (m_Mesh) {
            m_Mesh->Set(spD3DDeviceContext1, g_DRAWSTATE->m_isSkinningOn);
        }

        // Set input layout
        spD3DDeviceContext1->IASetInputLayout(m_spVertexLayout.Get());

        // Update per-object constant buffer
        ConstantBufferPerObjectVS cbPerObjectVS;
        XMStoreFloat4x4(&(cbPerObjectVS.World), XMMatrixTranspose(XMLoadFloat4x4(&m_World)));
        spD3DDeviceContext1->UpdateSubresource(spConstantBufferPerObjectVS.Get(), 0, nullptr, &cbPerObjectVS, 0, 0);


        ConstantBufferPerObjectPS cbPerObjectPS;
        cbPerObjectPS.SpecularExponent = m_SpecularExponent;
        cbPerObjectPS.SpecularIntensity = m_SpecularIntensity;
        spD3DDeviceContext1->UpdateSubresource(spConstantBufferPerObjectPS.Get(), 0, nullptr, &cbPerObjectPS, 0, 0);

        BindShaderResources(spD3DDeviceContext1, resources);

        spD3DDeviceContext1->VSSetShader(spVertexShader.Get(), nullptr, 0);
        spD3DDeviceContext1->PSSetShader(spPixelShader.Get(), nullptr, 0);

        if (m_Mesh) {
            m_Mesh->Draw(spD3DDeviceContext1);
        }
    }

    void Model::DrawBones(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1Factory> &spD2DFactory, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectVS, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectPS) const{
        if (m_Controller)
        {
            /*for (UINT b = 0; b < m_Controller->m_pSkeleton->m_Bones.size(); ++b)
            {
                Bone& bone = m_Controller->m_pSkeleton->m_Bones[b];
                for (UINT c = 0; c < bone.Children.size(); ++c)
                {
                    // Convert from world to screen coordinates
                    XMFLOAT2 points[2];
                    points[0] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(GetPosition(m_Controller->m_BoneMatrixBuffer[bone.BoneIndex]), m_World);
                    points[1] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(GetPosition(m_Controller->m_BoneMatrixBuffer[bone.Children[c]->BoneIndex]), m_World);

                    // Use D2D to draw a line in screen coordinates
                    spD2DDeviceContext->DrawLine(
                        D2D1::Point2F(points[0].x, points[0].y),
                        D2D1::Point2F(points[1].x, points[1].y),
                        g_GRAPHICSSYSTEM->GetD2DBrush(ColorOrange).Get());
                }
            }*/
            if (m_Controller && g_DRAWSTATE->m_drawingMode == DebugDrawingMode::Skeleton)
            {
                //m_Controller->RenderSkeleton(spD2DDeviceContext);
                m_Controller->RenderSkeleton2D(spD2DDeviceContext);
            }
        }
    }

    void Model::DrawDebug(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1Factory> &spD2DFactory){
        // Get the position of the object
        Transform* Pos = GetOwner()->has(Transform);

        // Get the position of the player
        GameObject* Player = g_FACTORY->GetObjList().FindByName("Player");
        Transform* pPos = Player->has(Transform);

        // Calculate the coordinates in 2D screen space
        float scale = 20.0f;
        float WindowCoordX = (Pos->GetPosition().x - pPos->GetPosition().x)*scale + (float)g_GRAPHICSSYSTEM->GetWindowSize().x / 2.0f;
        float WindowCoordY = (pPos->GetPosition().y - Pos->GetPosition().y)*scale + (float)g_GRAPHICSSYSTEM->GetWindowSize().y / 2.0f;
        
        // Choose the brush color based on the z-depth of the object
        D2D1_COLOR_F BrushColor;
        RigidBody* p = GetOwner()->has(RigidBody);
        if(!p)
            BrushColor = D2D1::ColorF(D2D1::ColorF::White);
        else if(Pos->GetPosition().z > 0.5f)
            BrushColor = D2D1::ColorF(D2D1::ColorF::Red);
        else if(Pos->GetPosition().z < -0.5f)
            BrushColor = D2D1::ColorF(D2D1::ColorF::Yellow);
        else
            BrushColor = D2D1::ColorF(D2D1::ColorF::Orange);

        ComPtr<ID2D1SolidColorBrush> spSolidBrush;
        spD2DDeviceContext->CreateSolidColorBrush(
            BrushColor,
            &spSolidBrush);

        // Draw the Rect
        spD2DDeviceContext->DrawRectangle(
            D2D1::RectF(
                WindowCoordX - m_Size.x * 0.5f * scale,
                WindowCoordY - m_Size.y * 0.5f * scale,
                WindowCoordX + m_Size.x * 0.5f * scale,
                WindowCoordY + m_Size.y * 0.5f * scale),
            spSolidBrush.Get());

        // If the model has a physics component, create and draw a line to indicate velocity
        if(p){
            // Create Geometry for the velocity
            ComPtr<ID2D1PathGeometry> spVelocityGeometry;
            DXThrowIfFailed(spD2DFactory->CreatePathGeometry(&spVelocityGeometry));
            ComPtr<ID2D1GeometrySink> spGeometrySink;
            DXThrowIfFailed(spVelocityGeometry->Open(&spGeometrySink));

            spGeometrySink->BeginFigure(
                D2D1::Point2F(WindowCoordX, WindowCoordY),
                D2D1_FIGURE_BEGIN_FILLED);

            spGeometrySink->AddLine(
                D2D1::Point2F(
                p->velocity.x * 3.0f + WindowCoordX,
                WindowCoordY - p->velocity.y * 3.0f));

            spGeometrySink->EndFigure(D2D1_FIGURE_END_OPEN);

            DXThrowIfFailed(spGeometrySink->Close());

            // Draw the velocity geometry
            spD2DDeviceContext->DrawGeometry(spVelocityGeometry.Get(), spSolidBrush.Get());
        }
    }

    void Model::LoadBinaryModel(const std::string &filename, const ComPtr<ID3D11Device1> &spD3DDevice1){
        ChunkReader file;
        file.Open(filename);

        //TODO: Expand loading code to be more generic

        GChunk fileChunk = file.ReadChunkHeader();

        ThrowErrorIf(fileChunk.Type != FileMark, "Invalid Model file");
           
        this->SetModelType(ModelTypeStatic);

        fileChunk = file.PeekChunk();

        while (fileChunk.Type != 0)
        {
            switch (fileChunk.Type)
            {
            case MarkMesh:
            {
                this->m_Mesh = new Mesh();
                this->m_Mesh->ReadFrom(file);
                this->m_Mesh->Initialize(spD3DDevice1);
            }
                break;
            case MarkSkel:
            {
                this->SetModelType(ModelTypeSkinned);
                this->m_Controller = new AnimationController();

                Skeleton * pNewSkeleton = new Skeleton();
                pNewSkeleton->ReadFrom(file);
                pNewSkeleton->Initialize();
                this->m_Controller->SetSkeleton(pNewSkeleton);
            }
                break;
            case MarkAnimation:
            {
                Animation * pNewAnimation = new Animation();
                pNewAnimation->ReadFrom(file);
                this->m_Controller->AddAnimation(pNewAnimation);
            }
                break;
            default:
                file.SkipChunk(fileChunk);
                break;
            }

            fileChunk = file.PeekChunk();
        }
    }
}
