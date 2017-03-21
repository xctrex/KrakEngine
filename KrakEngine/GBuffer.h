/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GBuffer.h
Purpose: Header for GBuffer.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 2/9/2014
- End Header -----------------------------------------------------*/


#pragma once
#include "Precompiled.h"
#include "Vertex.h"

namespace KrakEngine{

    class GBuffer{

	public:
        GBuffer();
        ~GBuffer(){};

        void Initialize(const ComPtr<ID3D11Device1> &spD3DDevice1, D3D11_TEXTURE2D_DESC &bufferDesc);
	    void TargetGBuffer(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const;
        void UnbindTargets(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1);
        void BindInput(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11SamplerState> &spSampler, const ComPtr<ID3D11SamplerState> &spWrapSampler) const;
        void UnbindInput(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const;
        void DrawVisualization(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11VertexShader> &spVertexShader, const ComPtr<ID3D11PixelShader> &spPixelShader) const;

        void PrepareForUnpack(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11Buffer> &spConstantBuffer, XMFLOAT4X4* pViewMatrix, XMFLOAT4X4* pProjectionMatrix);

        ID3D11DepthStencilView* GetReadOnlyDSV() const { return m_spDepthStencilViewReadOnly.Get(); }
        ID3D11DepthStencilView* GetDepthStencilView() const { return m_spDepthStencilView.Get(); }
        ID3D11DepthStencilState* GetDepthStencilState() const { return m_spDepthStencilState.Get(); }
        ID3D11DepthStencilState* GetNoDepthWriteLessStencilMaskState() const { return m_spNoDepthWriteLessStencilMaskState.Get(); }

        ID3D11ShaderResourceView* GetDepthView() const { return m_spDepthStencilSRV.Get(); }
        ID3D11ShaderResourceView*const* GetDepthViewAddress() const { return m_spDepthStencilSRV.GetAddressOf(); }
        ID3D11ShaderResourceView* GetColorSpecularIntensityView() const { return m_spColorSpecularIntensitySRV.Get(); }
        ID3D11ShaderResourceView* GetNormalView() const { return m_spNormalSRV.Get(); }
        ID3D11ShaderResourceView* GetSpecularExponentView() const { return m_spPositionSRV.Get(); }

        
    private:
        // GBuffer textures
        ComPtr<ID3D11Texture2D> m_spDepthStencilRT;
        ComPtr<ID3D11Texture2D> m_spColorSpecularIntensityRT;
        ComPtr<ID3D11Texture2D> m_spNormalRT;
        ComPtr<ID3D11Texture2D> m_spPositonRT;

        // GBuffer render target views
        ComPtr<ID3D11DepthStencilView> m_spDepthStencilView;
        ComPtr<ID3D11DepthStencilView> m_spDepthStencilViewReadOnly;
	    ComPtr<ID3D11RenderTargetView> m_spColorSpecularIntensityRTV;
	    ComPtr<ID3D11RenderTargetView> m_spNormalRTV;
	    ComPtr<ID3D11RenderTargetView> m_spPositionRTV;

        // GBuffer shader resource views
        ComPtr<ID3D11ShaderResourceView> m_spDepthStencilSRV;
        ComPtr<ID3D11ShaderResourceView> m_spColorSpecularIntensitySRV;
        ComPtr<ID3D11ShaderResourceView> m_spNormalSRV;
        ComPtr<ID3D11ShaderResourceView> m_spPositionSRV;

        //
        // Texture, Render View, and Resource View formats
        //

        // Depth Stencil 
        ComPtr<ID3D11DepthStencilState> m_spDepthStencilState;
        ComPtr<ID3D11DepthStencilState> m_spNoDepthWriteLessStencilMaskState;
        static const DXGI_FORMAT m_DepthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
        static const DXGI_FORMAT m_DepthStencilRTVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        static const DXGI_FORMAT m_DepthStencilSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

        // Color and Specular Intensity
        static const DXGI_FORMAT m_ColorSpecularIntensityTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT m_ColorSpecularIntensityRTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT m_ColorSpecularIntensitySRVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

        // Normal
        static const DXGI_FORMAT m_NormalTextureFormat = DXGI_FORMAT_R11G11B10_FLOAT;
        static const DXGI_FORMAT m_NormalRTVFormat = DXGI_FORMAT_R11G11B10_FLOAT;
        static const DXGI_FORMAT m_NormalSRVFormat = DXGI_FORMAT_R11G11B10_FLOAT;

        // Specular Exponent
        static const DXGI_FORMAT m_SpecularExponentTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT m_SpecularExponentRTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT m_SpecularExponentSRVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	};
}
