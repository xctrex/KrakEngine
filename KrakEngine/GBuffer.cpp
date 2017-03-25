/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GBuffer.cpp
Purpose: Implementation for class containing GBuffer data
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 2/9/2014
- End Header -----------------------------------------------------*/

#include "GBuffer.h"
#include "GraphicsSystem.h"

namespace KrakEngine{
    GBuffer::GBuffer(){}
    
    void GBuffer::Initialize(const ComPtr<ID3D11Device1> &spD3DDevice1, D3D11_TEXTURE2D_DESC &bufferDesc){
        //
        // Depth Stencil
        //

        // Create the depth stencil render target
        bufferDesc.Format = m_DepthStencilTextureFormat;
        bufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.MipLevels = 1;
        bufferDesc.ArraySize = 1;

        DXThrowIfFailed(
            spD3DDevice1->CreateTexture2D(
                &bufferDesc,
                nullptr,
                &m_spDepthStencilRT
                )
            );

        // Create the depth stencil render target view
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilRTVDesc = {
            m_DepthStencilRTVFormat,
            D3D11_DSV_DIMENSION_TEXTURE2D,
            0
        };

        DXThrowIfFailed(
            spD3DDevice1->CreateDepthStencilView(
                m_spDepthStencilRT.Get(),
                &depthStencilRTVDesc,
                &m_spDepthStencilView
                )
            );

        // Create read only depth stencil render target view
        depthStencilRTVDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
        DXThrowIfFailed(
            spD3DDevice1->CreateDepthStencilView(
                m_spDepthStencilRT.Get(),
                &depthStencilRTVDesc,
                &m_spDepthStencilViewReadOnly
                )
            );
        
        // Create the depth stencil shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {
            m_DepthStencilSRVFormat,
            D3D11_SRV_DIMENSION_TEXTURE2D,
            0,
            0            
        };
        SRVDesc.Texture2D.MipLevels = 1;
        DXThrowIfFailed(
            spD3DDevice1->CreateShaderResourceView(
                m_spDepthStencilRT.Get(),
                &SRVDesc,
                &m_spDepthStencilSRV
                )
            );

        // Create the depth stencil state
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = TRUE;
        depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
        depthStencilDesc.FrontFace = stencilMarkOp;
        depthStencilDesc.BackFace = stencilMarkOp;

        DXThrowIfFailed(
            spD3DDevice1->CreateDepthStencilState(&depthStencilDesc, m_spDepthStencilState.GetAddressOf())
        );

        // Create NoDepthWriteLessStencilMaskState
        D3D11_DEPTH_STENCIL_DESC noWriteDepthStencilDesc;
        noWriteDepthStencilDesc.DepthEnable = TRUE;
        noWriteDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        noWriteDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        noWriteDepthStencilDesc.StencilEnable = TRUE;
        noWriteDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        noWriteDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
        noWriteDepthStencilDesc.FrontFace = noSkyStencilOp;
        noWriteDepthStencilDesc.BackFace = noSkyStencilOp;
        DXThrowIfFailed(
            spD3DDevice1->CreateDepthStencilState(&noWriteDepthStencilDesc, m_spNoDepthWriteLessStencilMaskState.GetAddressOf())
            );

        //
        // Color and Specular Intensity
        //

        // Create the color and specular intensity render target
        bufferDesc.Format = m_ColorSpecularIntensityTextureFormat;
        bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        DXThrowIfFailed(
            spD3DDevice1->CreateTexture2D(
                &bufferDesc,
                nullptr,
                &m_spColorSpecularIntensityRT
                )
            );

        // Create the color and specular intensity render target view
        CD3D11_RENDER_TARGET_VIEW_DESC RTVDesc(
            D3D11_RTV_DIMENSION_TEXTURE2D,
            m_ColorSpecularIntensityRTVFormat
            );

        DXThrowIfFailed(
            spD3DDevice1->CreateRenderTargetView(
                m_spColorSpecularIntensityRT.Get(),
                &RTVDesc,
                &m_spColorSpecularIntensityRTV
                )
            );
        
        // Create the color and specular intensity shader resource view
        SRVDesc.Format = m_ColorSpecularIntensitySRVFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateShaderResourceView(
                m_spColorSpecularIntensityRT.Get(),
                &SRVDesc,
                &m_spColorSpecularIntensitySRV
                )
            );

        //
        // Normal
        //

        // Create the normal render target
        bufferDesc.Format = m_NormalTextureFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateTexture2D(
                &bufferDesc,
                nullptr,
                &m_spNormalRT
                )
            );

        // Create the normal render target view
        RTVDesc.Format = m_NormalRTVFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateRenderTargetView(
                m_spNormalRT.Get(),
                &RTVDesc,
                &m_spNormalRTV
                )
            );
        
        // Create the normal shader resource view
        SRVDesc.Format = m_NormalSRVFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateShaderResourceView(
                m_spNormalRT.Get(),
                &SRVDesc,
                &m_spNormalSRV
                )
            );

        //
        // Specular Exponent
        //

        // Create the specular exponent render target
        bufferDesc.Format = m_SpecularExponentTextureFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateTexture2D(
                &bufferDesc,
                nullptr,
                &m_spPositonRT
                )
            );

        // Create the specular exponent render target view
        RTVDesc.Format = m_SpecularExponentRTVFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateRenderTargetView(
                m_spPositonRT.Get(),
                &RTVDesc,
                &m_spPositionRTV
                )
            );
        
        // Create the specular exponent shader resource view
        SRVDesc.Format = m_SpecularExponentSRVFormat;

        DXThrowIfFailed(
            spD3DDevice1->CreateShaderResourceView(
                m_spPositonRT.Get(),
                &SRVDesc,
                &m_spPositionSRV
                )
            );
    }

    void GBuffer::TargetGBuffer(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const{
        // Clear the depth stencil
        spD3DDeviceContext1->ClearDepthStencilView( m_spDepthStencilView.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0, 0 );

        float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        // Clear the render target views
        spD3DDeviceContext1->ClearRenderTargetView(m_spColorSpecularIntensityRTV.Get(), ClearColor);
        spD3DDeviceContext1->ClearRenderTargetView(m_spNormalRTV.Get(), ClearColor);
        spD3DDeviceContext1->ClearRenderTargetView(m_spPositionRTV.Get(), ClearColor);

        // Bind the render targets together
        ID3D11RenderTargetView* rt[3] = { m_spColorSpecularIntensityRTV.Get(), m_spNormalRTV.Get(), m_spPositionRTV.Get() };
        spD3DDeviceContext1->OMSetRenderTargets(3, rt, m_spDepthStencilView.Get());
    
        spD3DDeviceContext1->OMSetDepthStencilState(m_spDepthStencilState.Get(), 1);
    }

    void GBuffer::TargetPositionLuminanceBuffer(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const {
        // Bind the position-luminance render target
        spD3DDeviceContext1->OMSetRenderTargets(1, m_spPositionRTV.GetAddressOf(), nullptr);
    }

    void GBuffer::UnbindTargets(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1){
        spD3DDeviceContext1->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
               
        ID3D11RenderTargetView* views[4] = {NULL, NULL, NULL, NULL};
        spD3DDeviceContext1->OMSetRenderTargets(ARRAYSIZE(views), views, m_spDepthStencilViewReadOnly.Get());
    }

    void GBuffer::BindInput(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11SamplerState> &spSampler, const ComPtr<ID3D11SamplerState> &spWrapSampler) const{
        // Bind the Resource Views
        ID3D11ShaderResourceView* views[4] = {m_spDepthStencilSRV.Get(), m_spColorSpecularIntensitySRV.Get(), m_spNormalSRV.Get(), m_spPositionSRV.Get()};
        spD3DDeviceContext1->PSSetShaderResources(0, ARRAYSIZE(views), views);

        ID3D11SamplerState* samplers[2] = { spSampler.Get(), spWrapSampler.Get() };
        spD3DDeviceContext1->PSSetSamplers( 0, ARRAYSIZE(samplers), samplers );
    }

    void GBuffer::UnbindInput(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1) const{
        // Cleanup
        ID3D11ShaderResourceView* views[4] = {m_spDepthStencilSRV.Get(), m_spColorSpecularIntensitySRV.Get(), m_spNormalSRV.Get(), m_spPositionSRV.Get()};
        //ID3D11ShaderResourceView* views[3] = {m_spDepthStencilSRV.Get(), m_spColorSpecularIntensitySRV.Get(), m_spNormalSRV.Get()};
        ZeroMemory(views, sizeof(views));
        spD3DDeviceContext1->PSSetShaderResources(0, ARRAYSIZE(views), views);
    }

    void GBuffer::PrepareForUnpack(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11Buffer> &spConstantBuffer, XMFLOAT4X4* pViewMatrix, XMFLOAT4X4* pProjectionMatrix){
        // Fill the constant buffer that is used to unpack the GBuffer
        ConstantBufferGBufferUnpack cb;
        
        // Store the values of the perspective transform for the constant buffer
        cb.PerspectiveValues.x = 1.0f / pProjectionMatrix->m[0][0];
        cb.PerspectiveValues.y = 1.0f / pProjectionMatrix->m[1][1];
        cb.PerspectiveValues.z = pProjectionMatrix->m[3][2];
        cb.PerspectiveValues.w = -pProjectionMatrix->m[2][2];

        // Calculate the view inverse for the constant buffer
        XMMATRIX viewInverse;
        viewInverse = XMMatrixInverse(NULL, XMLoadFloat4x4(pViewMatrix));
        XMStoreFloat4x4(&(cb.InverseViewTransform), XMMatrixTranspose(viewInverse));

        // Update the constant buffer and set it to the pixel shader
        spD3DDeviceContext1->UpdateSubresource(spConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
        
        spD3DDeviceContext1->PSSetConstantBuffers(2, 1, spConstantBuffer.GetAddressOf());
    }

    void GBuffer::DrawVisualization(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ComPtr<ID3D11VertexShader> &spVertexShader, const ComPtr<ID3D11PixelShader> &spPixelShader) const{
        spD3DDeviceContext1->IASetInputLayout(NULL);
        spD3DDeviceContext1->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
        spD3DDeviceContext1->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            
        // Set the shaders
        spD3DDeviceContext1->VSSetShader(spVertexShader.Get(), NULL, 0);
        spD3DDeviceContext1->GSSetShader(NULL, NULL, 0);
        spD3DDeviceContext1->PSSetShader(spPixelShader.Get(), NULL, 0);
        spD3DDeviceContext1->Draw(24, 0);
    }
}
