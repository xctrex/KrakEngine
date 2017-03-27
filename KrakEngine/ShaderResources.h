#pragma once
#include "Precompiled.h"

struct ShaderResources
{
    ShaderResources(
        std::vector<ID3D11ShaderResourceView*> vertexTextures,
        std::vector<ID3D11SamplerState*> vertexSamplers,
        std::vector<ID3D11ShaderResourceView*> pixelTextures,
        std::vector<ID3D11SamplerState*> pixelSamplers)
        : vertexShaderTextures(vertexTextures)
        , vertexShaderSamplers(vertexSamplers)
        , pixelShaderTextures(pixelTextures)
        , pixelShaderSamplers(pixelSamplers)
    {}

    std::vector<ID3D11ShaderResourceView*> vertexShaderTextures;
    std::vector<ID3D11SamplerState*> vertexShaderSamplers;

    std::vector<ID3D11ShaderResourceView*> pixelShaderTextures;
    std::vector<ID3D11SamplerState*> pixelShaderSamplers;
};

void BindShaderResources(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ShaderResources &resources);
void UnBindShaderResources(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ShaderResources &resources);