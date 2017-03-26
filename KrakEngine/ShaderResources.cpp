#include "ShaderResources.h"

void BindShaderResources(const ComPtr<ID3D11DeviceContext1> &spD3DDeviceContext1, const ShaderResources &resources)
{
    if (resources.vertexShaderTextures.size() > 0)
    {
        spD3DDeviceContext1->VSSetShaderResources(0, resources.vertexShaderTextures.size(), resources.vertexShaderTextures.data());
    }
    if (resources.vertexShaderSamplers.size() > 0)
    {
        spD3DDeviceContext1->VSSetSamplers(0, resources.vertexShaderSamplers.size(), resources.vertexShaderSamplers.data());
    }
    if (resources.pixelShaderTextures.size() > 0)
    {
        spD3DDeviceContext1->PSSetShaderResources(0, resources.pixelShaderTextures.size(), resources.pixelShaderTextures.data());
    }
    if (resources.pixelShaderSamplers.size() > 0)
    {
        spD3DDeviceContext1->PSSetSamplers(0, resources.pixelShaderSamplers.size(), resources.pixelShaderSamplers.data());
    }
}