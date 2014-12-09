/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Header for Model
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 1/20/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "Component.h"
#include "GraphicsSystem.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Animation.h"

namespace KrakEngine{

	class Transform;
    class RigidBody;

    enum ModelType
    {
        ModelTypeStatic,
        ModelTypeSkinned
    };
    
	class Model : public Component{

	public:
        Model();
		~Model() {};
		virtual void Initialize();
		virtual void Shutdown();
		virtual void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);

        void Skin(const ComPtr<ID3D11Device1> &spD3DDevice1, const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1);
        void Draw(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11Buffer> &spConstantBufferPerObject, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectPS) const;

        void DrawBones(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1Factory> &spD2DFactory, const ComPtr<ID3D11Buffer> &spConstantBufferPerObject, const ComPtr<ID3D11Buffer> &spConstantBufferPerObjectPS) const;

        void DrawDebug(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1Factory> &spD2DFactory);
        
        void CreateBuffers(const ComPtr<ID3D11Device1> &spD3DDevice1);
        void Update(float dt);

        // Get and Set functions
        XMFLOAT3 GetSize(){return m_Size;}
        void SetSize(float x, float y, float z){
            m_Size.x = x;
            m_Size.y = y;
            m_Size.z = z;
        }

		std::string GetModelName() {return m_ModelName; }
		void SetModelName(std::string newname) { m_ModelName = newname; }

		void SetSize(const XMFLOAT3 & newsize)
		{
			m_Size = newsize;
		}

        XMFLOAT4X4 GetWorldTransform()const {return m_World;}

		bool IsSelected() {return m_IsSelected;}
		void SetSelected(bool b) {m_IsSelected = b;}

		void SetTextureName(std::string texture) {m_TextureName = texture;}
		std::string GetTextureName() {return m_TextureName;}

        void SetModelType(ModelType type){ m_ModelType = type; }
        void LoadBinaryModel(const std::string &filename, const ComPtr<ID3D11Device1> &spD3DDevice1);

		Model * Next;
		Model * Prev;

        Mesh * m_Mesh;
        AnimationController * m_Controller;
    private:
        ModelType m_ModelType;

        ComPtr<ID3D11Buffer> m_spVertexBuffer;
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
		XMFLOAT3 m_Size;
        float m_SpecularExponent;
        float m_SpecularIntensity;
        std::string m_ModelName;

		bool m_IsSelected;
        VertexType m_VertexType;
        std::string m_TextureName;
        XMFLOAT3 m_OffsetRotation;
        XMFLOAT3 m_OffsetPosition;
	};

}
