/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Sprite.cpp
Purpose: Component for Sprites
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Sprite.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "Factory.h"
#include "RigidBody.h"

namespace KrakEngine{

    Sprite::Sprite() :
        Component(SpriteComp),
        m_NumIndices(6),
        m_Stride(36),
        m_Offset(0),
        m_Size(1.0f, 1.0f),
        m_FrameHeight(1.0f),
        m_FrameWidth(1.0f),
        m_FrameOffsetX(0.0f),
        m_FrameOffsetY(0.0f),
        m_Time(0.0f),
		m_IsSelected(false),
		m_DontDraw(false),
		m_IsFacingRight(true),
        m_AnimationState(AnimationStateIdle),
        m_bPauseAnimation(false),
        m_bIsBackgroundSprite(false)
    {
        m_IdleAnimation.Length = 1.0f;
        m_IdleAnimation.NumFrames = 1.0f;
        m_IdleAnimation.StartFrame = 0.0f;
        m_RunAnimation = m_JumpAnimation = m_IdleAnimation;
        CreateBuffers(g_GRAPHICSSYSTEM->GetD3DDevice1());
        g_GRAPHICSSYSTEM->AddSpriteToList(this);
    }

    void Sprite::Initialize(){}

    void Sprite::Shutdown(){}

    void Sprite::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, m_Size.x, "SizeX");
        InitializeDataMember(stream, m_Size.y, "SizeY");
        InitializeDataMember(stream, m_TextureName, "TextureName");

        // Animation
        InitializeDataMember(stream, m_IdleAnimation.Length, "IdleAnimationLength");
        InitializeDataMember(stream, m_IdleAnimation.NumFrames, "IdleAnimationNumFrames");
        InitializeDataMember(stream, m_IdleAnimation.StartFrame, "IdleAnimationStartFrame");
        InitializeDataMember(stream, m_RunAnimation.Length, "RunAnimationLength");
        InitializeDataMember(stream, m_RunAnimation.NumFrames, "RunAnimationNumFrames");
        InitializeDataMember(stream, m_RunAnimation.StartFrame, "RunAnimationStartFrame");
        InitializeDataMember(stream, m_JumpAnimation.Length, "JumpAnimationLength");
        InitializeDataMember(stream, m_JumpAnimation.NumFrames, "JumpAnimationNumFrames");
        InitializeDataMember(stream, m_JumpAnimation.StartFrame, "JumpAnimationStartFrame");
        InitializeDataMember(stream, m_FrameWidth, "FrameWidth");
        InitializeDataMember(stream, m_FrameHeight, "FrameHeight");
		InitializeDataMember(stream, m_IsFacingRight, "Right");
		InitializeDataMember(stream, m_bIsBackgroundSprite, "BackgroundSprite");
        
        m_spVertexLayout = g_GRAPHICSSYSTEM->GetSpriteVertexLayout();
        m_spVertexShader = g_GRAPHICSSYSTEM->GetSpriteVertexShader();
        m_spPixelShader = g_GRAPHICSSYSTEM->GetSpritePixelShader();
        m_spSampler = g_GRAPHICSSYSTEM->GetSampler();

        /*if(!m_bIsBackgroundSprite){
            g_GRAPHICSSYSTEM->AddSpriteToList(this);
        }
        else{
            g_GRAPHICSSYSTEM->AddSpriteToBackgroundList(this);
        }*/
    }

	void Sprite::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_Size.x, "SizeX");
        WriteDataMember(element, m_Size.y, "SizeY");
        WriteDataMember(element, m_TextureName, "TextureName");

        // Animation
        WriteDataMember(element, m_IdleAnimation.Length, "IdleAnimationLength");
        WriteDataMember(element, m_IdleAnimation.NumFrames, "IdleAnimationNumFrames");
        WriteDataMember(element, m_IdleAnimation.StartFrame, "IdleAnimationStartFrame");
        WriteDataMember(element, m_RunAnimation.Length, "RunAnimationLength");
        WriteDataMember(element, m_RunAnimation.NumFrames, "RunAnimationNumFrames");
        WriteDataMember(element, m_RunAnimation.StartFrame, "RunAnimationStartFrame");
        WriteDataMember(element, m_JumpAnimation.Length, "JumpAnimationLength");
        WriteDataMember(element, m_JumpAnimation.NumFrames, "JumpAnimationNumFrames");
        WriteDataMember(element, m_JumpAnimation.StartFrame, "JumpAnimationStartFrame");
        WriteDataMember(element, m_FrameWidth, "FrameWidth");
        WriteDataMember(element, m_FrameHeight, "FrameHeight");        
		WriteDataMember(element, m_IsFacingRight, "Right");
		WriteDataMember(element, m_bIsBackgroundSprite, "BackgroundSprite");

	}

    void Sprite::CreateBuffers(const ComPtr<ID3D11Device1> &spD3DDevice1){        
	    // Create vertex buffer
	    SpriteVertex vertices[] =
        {
            { XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // Blue (top left)
            { XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, // Green (top right)
            { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, // Pink (bottom left)
            { XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // Yellow (bottom right)
	    };
	    D3D11_BUFFER_DESC bd;
	    ZeroMemory(&bd, sizeof(bd));
	    bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SpriteVertex) * ARRAYSIZE(vertices);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	    bd.CPUAccessFlags = 0;

	    D3D11_SUBRESOURCE_DATA InitData;
	    ZeroMemory(&InitData, sizeof(InitData));
	    InitData.pSysMem = vertices;
	    DXThrowIfFailed(
		    spD3DDevice1->CreateBuffer(
			    &bd, 
			    &InitData, 
                m_spVertexBuffer.GetAddressOf()));

        // Create flipped vertex buffer
	    SpriteVertex flippedvertices[] =
        {
            { XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, // Blue (top left)
            { XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // Green (top right)
            { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // Pink (bottom left)
            { XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, // Yellow (bottom right)
	    };
	    ZeroMemory(&bd, sizeof(bd));
	    bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SpriteVertex) * ARRAYSIZE(flippedvertices);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	    bd.CPUAccessFlags = 0;

	    ZeroMemory(&InitData, sizeof(InitData));
	    InitData.pSysMem = flippedvertices;
	    DXThrowIfFailed(
		    spD3DDevice1->CreateBuffer(
			    &bd, 
			    &InitData, 
                m_spFlippedVertexBuffer.GetAddressOf()));

	    // Set vertex buffer
	    m_Stride = sizeof(SpriteVertex);
	    m_Offset = 0;
       
	    // Create index buffer
	    WORD indices[] =
	    {
            // Quad
		    0, 3, 2,
		    1, 3, 0,
	    };
	    bd.Usage = D3D11_USAGE_DEFAULT;
	    bd.ByteWidth = sizeof(WORD)* ARRAYSIZE(indices);
	    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	    bd.CPUAccessFlags = 0;
	    InitData.pSysMem = indices;
	    DXThrowIfFailed(
		    spD3DDevice1->CreateBuffer(&bd, &InitData, m_spIndexBuffer.ReleaseAndGetAddressOf()));
    }

    void Sprite::Update(float dt){
        
        switch(m_AnimationState)
        {
            case AnimationStateIdle :
            {
                UpdateAnimation(dt, m_IdleAnimation);
                break;
            }
            case AnimationStateRunning :
            {
                UpdateAnimation(dt, m_RunAnimation);
                break;
            }
            case AnimationStateJumping :
            {
                UpdateAnimation(dt, m_JumpAnimation);
                break;
            }
            default:
                break;
        }
        if(GetOwner()){
            Transform* t = GetOwner()->has(Transform);

            XMStoreFloat4x4(&m_World, XMMatrixScaling(m_Size.x, m_Size.y, 0.0f) * XMMatrixRotationY(t->GetRotation().y) * XMMatrixTranslation(t->GetPosition().x, t->GetPosition().y, t->GetPosition().z));
        }
    }

    void Sprite::UpdateAnimation(float dt, SpriteAnimation animation){
        if(!m_bPauseAnimation){
            // Update the time
            m_Time += dt;
            // Loop back to the beginning of the animation if the time has run past the animation length
            if(m_Time > animation.Length)
                m_Time -= animation.Length;

            // Verify there is a valid number of frames
            ThrowErrorIf(animation.NumFrames <= 0.0f, "The animation must have a non-zero, positive number of frames");

            float timePerFrame = animation.Length / animation.NumFrames;
            UINT numxframes = (UINT)(1.0f / m_FrameWidth);
            UINT currentFrame = (UINT)(m_Time / timePerFrame) + animation.StartFrame;

            UINT currentXFrame = currentFrame % numxframes;
            UINT currentYFrame = currentFrame / numxframes;

            m_FrameOffsetX = m_FrameWidth * currentXFrame;
            m_FrameOffsetY = m_FrameHeight * currentYFrame;
        }
    }

    void Sprite::Draw(
        const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1,
        const ComPtr<ID3D11Buffer> &spConstantBufferPerObject){

		if(m_DontDraw)
			return;

        // Set vertex buffer
        if(m_IsFacingRight){
            spD3DDeviceContext1->IASetVertexBuffers(0, 1, m_spVertexBuffer.GetAddressOf(), &m_Stride, &m_Offset);
        }
        else{
            spD3DDeviceContext1->IASetVertexBuffers(0, 1, m_spFlippedVertexBuffer.GetAddressOf(), &m_Stride, &m_Offset);
        }
        
        // Set index buffer
        spD3DDeviceContext1->IASetIndexBuffer(m_spIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        spD3DDeviceContext1->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        // Set input layout
        spD3DDeviceContext1->IASetInputLayout(m_spVertexLayout.Get());

        // Update per-object constant buffer
        ConstantBufferPerSpriteObject cbPerObject;
        XMStoreFloat4x4(&(cbPerObject.World), XMMatrixTranspose(XMLoadFloat4x4(&m_World)));
        cbPerObject.Animation.x = m_FrameOffsetX;
        cbPerObject.Animation.y = m_FrameWidth;
        cbPerObject.Animation.z = m_FrameOffsetY;
        cbPerObject.Animation.w = m_FrameHeight;

        spD3DDeviceContext1->UpdateSubresource(spConstantBufferPerObject.Get(), 0, nullptr, &cbPerObject, 0, 0);
        spD3DDeviceContext1->PSSetShaderResources( 0, 1, g_GRAPHICSSYSTEM->GetTexture(m_TextureName).GetAddressOf() );
        spD3DDeviceContext1->PSSetSamplers( 0, 1, m_spSampler.GetAddressOf() );

		// Render
        spD3DDeviceContext1->VSSetShader(m_spVertexShader.Get(), nullptr, 0);       
        spD3DDeviceContext1->PSSetShader(m_spPixelShader.Get(), nullptr, 0);
        spD3DDeviceContext1->DrawIndexed(m_NumIndices, 0, 0);
    }

    void Sprite::DrawDebug(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1Factory> &spD2DFactory){
        // Get the position of the object
        Transform* pObjectPos = GetOwner()->has(Transform);

        XMFLOAT2 screenCoordinateTopLeft = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(XMFLOAT3(0.0f - m_Size.x * 0.5f, 0.0f + m_Size.y * 0.5f, 0.0f), m_World);
        XMFLOAT2 screenCoordinateBottomRight = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(XMFLOAT3(0.0f + m_Size.x * 0.5f, 0.0f - m_Size.y * 0.5f, 0.0f), m_World);

        // Choose the brush color based on the z-depth of the object
        D2D1_COLOR_F BrushColor;
        RigidBody* p = GetOwner()->has(RigidBody);
        if(!p)
            BrushColor = D2D1::ColorF(D2D1::ColorF::White);
        else if(pObjectPos->GetPosition().z > 0.5f)
            BrushColor = D2D1::ColorF(D2D1::ColorF::Red);
        else if(pObjectPos->GetPosition().z < -0.5f)
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
                screenCoordinateTopLeft.x,
                screenCoordinateTopLeft.y,
                screenCoordinateBottomRight.x,
                screenCoordinateBottomRight.y),
                spSolidBrush.Get());

        // If the sprite has a physics component, create and draw a line to indicate velocity
        if(p){
            // Create Geometry for the velocity
            ComPtr<ID2D1PathGeometry> spVelocityGeometry;
            DXThrowIfFailed(spD2DFactory->CreatePathGeometry(&spVelocityGeometry));
            ComPtr<ID2D1GeometrySink> spGeometrySink;
            DXThrowIfFailed(spVelocityGeometry->Open(&spGeometrySink));

            spGeometrySink->BeginFigure(
                D2D1::Point2F(
                    (screenCoordinateTopLeft.x + screenCoordinateBottomRight.x) * 0.5f,
                    (screenCoordinateTopLeft.y + screenCoordinateBottomRight.y) * 0.5f),
                    D2D1_FIGURE_BEGIN_FILLED);

            spGeometrySink->AddLine(
                D2D1::Point2F(
                p->velocity.x * 3.0f + (screenCoordinateTopLeft.x + screenCoordinateBottomRight.x) * 0.5f,
                (screenCoordinateTopLeft.y + screenCoordinateBottomRight.y) * 0.5f - p->velocity.y * 3.0f));

            spGeometrySink->EndFigure(D2D1_FIGURE_END_OPEN);

            DXThrowIfFailed(spGeometrySink->Close());

            // Draw the velocity geometry
            spD2DDeviceContext->DrawGeometry(spVelocityGeometry.Get(), spSolidBrush.Get());
        }
    }


	void Sprite::SetSize(float x, float y)
	{
		m_Size.x = x;
		m_Size.y = y;
	}


	void Sprite::SetSize(XMFLOAT2 newsize)
	{
		m_Size = newsize;
	}
}
