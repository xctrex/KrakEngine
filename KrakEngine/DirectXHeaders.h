#pragma once

#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include "DDSTextureLoader\DDSTextureLoader.h"

using namespace DirectX;

// Throw on error //TODO: Convert DirectX error codes to exceptions.
inline void DXThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        // Set a breakpoint on this line to catch DX API errors.
        throw;
    }
}

inline XMFLOAT4X4 BuildTransform(XMFLOAT3& translation, XMFLOAT4& rotation)
{
    XMFLOAT4X4 matrix;
    XMStoreFloat4x4(&matrix, XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)));
    matrix._41 = translation.x;
    matrix._42 = translation.y;
    matrix._43 = translation.z;
    return matrix;
}

inline XMFLOAT3 GetPosition(XMFLOAT4X4& matrix)
{
    return XMFLOAT3(matrix._41, matrix._42, matrix._43);
}
