//--------------------------------------------------------------------------------------
// File: CommonStates.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#include <d3d12.h>

#include <memory>


namespace DirectX
{
    class CommonStates
    {
    public:
        explicit CommonStates(_In_ ID3D12Device* device);
        CommonStates(CommonStates&& moveFrom);
        CommonStates& operator= (CommonStates&& moveFrom);

        CommonStates(CommonStates const&) = delete;
        CommonStates& operator= (CommonStates const&) = delete;

        virtual ~CommonStates();

        // Blend states.
        D3D12_BLEND_DESC __cdecl Opaque() const;
        D3D12_BLEND_DESC __cdecl AlphaBlend() const;
        D3D12_BLEND_DESC __cdecl Additive() const;
        D3D12_BLEND_DESC __cdecl NonPremultiplied() const;

        // Depth stencil states.
        D3D12_DEPTH_STENCIL_DESC __cdecl DepthNone() const;
        D3D12_DEPTH_STENCIL_DESC __cdecl DepthDefault() const;
        D3D12_DEPTH_STENCIL_DESC __cdecl DepthRead() const;

        // Rasterizer states.
        D3D12_RASTERIZER_DESC __cdecl CullNone() const;
        D3D12_RASTERIZER_DESC __cdecl CullClockwise() const;
        D3D12_RASTERIZER_DESC __cdecl CullCounterClockwise() const;
        D3D12_RASTERIZER_DESC __cdecl Wireframe() const;

        // Sampler states.
        D3D12_SAMPLER_DESC __cdecl PointWrap() const;
        D3D12_SAMPLER_DESC __cdecl PointClamp() const;
        D3D12_SAMPLER_DESC __cdecl LinearWrap() const;
        D3D12_SAMPLER_DESC __cdecl LinearClamp() const;
        D3D12_SAMPLER_DESC __cdecl AnisotropicWrap() const;
        D3D12_SAMPLER_DESC __cdecl AnisotropicClamp() const;
        
        // Static sampler states.
        D3D12_STATIC_SAMPLER_DESC __cdecl PointWrap(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility) const;
        D3D12_STATIC_SAMPLER_DESC __cdecl PointClamp(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility) const;
        D3D12_STATIC_SAMPLER_DESC __cdecl LinearWrap(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility) const;
        D3D12_STATIC_SAMPLER_DESC __cdecl LinearClamp(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility) const;
        D3D12_STATIC_SAMPLER_DESC __cdecl AnisotropicWrap(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility) const;
        D3D12_STATIC_SAMPLER_DESC __cdecl AnisotropicClamp(UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility) const;

    private:
        // Private implementation.
        class Impl;

        std::shared_ptr<Impl> pImpl;
    };
}
