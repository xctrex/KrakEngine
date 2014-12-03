/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Sprite.h
Purpose: Header for Sprite
Language: C++, MSC
Platform: Windows 7
Project: go_ninja
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "component.h"
#include "GraphicsSystem.h"
#include "Vertex.h"

namespace KrakEngine{
    
    class Transform;
    class RigidBody;

    class Sprite : public Component{

    public:
	    Sprite();
        ~Sprite(){};
	    virtual void Initialize();
		virtual void Shutdown();
		virtual void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);
		
        void CreateBuffers(const ComPtr<ID3D11Device1> &spD3DDevice1);
        void Update(float dt);
        void Draw(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11Buffer> &spConstantBufferPerObject);

        void DrawDebug(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1Factory> &spD2DFactory);
        const ComPtr<ID3D11Texture2D>& GetTexture();

        void PauseAnimation(){ m_bPauseAnimation = true; }
        void UnPauseAnimation(){ m_bPauseAnimation = false; }

        XMFLOAT2 GetSize()const {return m_Size;}
        XMFLOAT4X4 GetWorldTransform()const {return m_World;}

	    Sprite * Next;
	    Sprite * Prev;

		bool IsSelected() {return m_IsSelected;}
		void SetSelected(bool b) {m_IsSelected = b;}
        void SetIsFacingRight(bool b) {m_IsFacingRight = b;}
        void SetAnimationState(AnimationState as) {m_AnimationState = as;}

		inline void SetTextureName(std::string newtextname) {m_TextureName = newtextname; }
        std::string GetTextureName() {return m_TextureName;}
		void SetSize(float x, float y);
		void SetSize(XMFLOAT2 newsize);

		void TurnDrawingOn() { m_DontDraw = false; }
		void TurnDrawingOff() { m_DontDraw = true; }

    private:
	    std::string m_TextureName;
        ComPtr<ID3D11SamplerState> m_spSampler;

        ComPtr<ID3D11Buffer> m_spVertexBuffer;
        ComPtr<ID3D11Buffer> m_spFlippedVertexBuffer;
	    ComPtr<ID3D11Buffer> m_spIndexBuffer;
        ComPtr<ID3D11InputLayout> m_spVertexLayout;
        ComPtr<ID3D11VertexShader> m_spVertexShader;
        ComPtr<ID3D11PixelShader> m_spPixelShader;
        UINT m_NumIndices;
        UINT m_Stride;
        UINT m_Offset;        
        XMFLOAT4X4	m_View;
        XMFLOAT4X4	m_Projection;
	    XMFLOAT4X4  m_World;
        
		XMFLOAT2 m_Size;
		
        // Animation data
        float m_FrameHeight;
        float m_FrameWidth;
        float m_FrameOffsetX;
        float m_FrameOffsetY;
        float m_Time;
        SpriteAnimation m_IdleAnimation;
        SpriteAnimation m_RunAnimation;
        SpriteAnimation m_JumpAnimation;
		SpriteAnimation m_None;
        AnimationState m_AnimationState;
        void UpdateAnimation(float dt, SpriteAnimation animation);

		bool m_IsSelected;
		bool m_DontDraw;
		bool m_IsFacingRight;
        bool m_bPauseAnimation;
        bool m_bIsBackgroundSprite;
    };
}
