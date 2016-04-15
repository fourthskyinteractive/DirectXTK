//--------------------------------------------------------------------------------------
// File: CommonStates.cpp
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

#include "pch.h"
#include "CommonStates.h"
#include "DemandCreate.h"
#include "DirectXHelpers.h"
#include "SharedResourcePool.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;


// Internal state object implementation class. Only one of these helpers is allocated
// per D3D device, even if there are multiple public facing CommonStates instances.
class CommonStates::Impl
{
public:
    Impl(_In_ ID3D12Device* device);
      : device(device)
    { }

    HRESULT CreateBlendState(D3D12_BLEND srcBlend, D3D12_BLEND destBlend, _Out_ D3D12_BLEND_DESC* pResult);
    HRESULT CreateDepthStencilState(bool enable, bool writeEnable, _Out_ D3D12_DEPTH_STENCIL_DESC* pResult);
    HRESULT CreateRasterizerState(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode, _Out_ D3D12_RASTERIZER_DESC* pResult);
    HRESULT CreateSamplerState(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode, _Out_ D3D12_CPU_DESCRIPTOR_HANDLE pResult);
	
	D3D12_STATIC_SAMPLER_DESC ParseSampler(const D3D12_SAMPLER_DESC& desc);

    ComPtr<ID3D12Device> device;

    static D3D12_BLEND_DESC opaque;
    static D3D12_BLEND_DESC alphaBlend;
    static D3D12_BLEND_DESC additive;
    static D3D12_BLEND_DESC nonPremultiplied;

    static D3D12_DEPTH_STENCIL_DESC depthNone;
    static D3D12_DEPTH_STENCIL_DESC depthDefault;
    static D3D12_DEPTH_STENCIL_DESC depthRead;

    static D3D12_RASTERIZER_DESC cullNone;
    static D3D12_RASTERIZER_DESC cullClockwise;
    static D3D12_RASTERIZER_DESC cullCounterClockwise;
    static D3D12_RASTERIZER_DESC wireframe;

    static D3D12_SAMPLER_DESC pointWrap;
    static D3D12_SAMPLER_DESC pointClamp;
    static D3D12_SAMPLER_DESC linearWrap;
    static D3D12_SAMPLER_DESC linearClamp;
    static D3D12_SAMPLER_DESC anisotropicWrap;
    static D3D12_SAMPLER_DESC anisotropicClamp;

    std::mutex mutex;

    static SharedResourcePool<ID3D12Device*, Impl> instancePool;
};


// Global instance pool.
SharedResourcePool<ID3D12Device*, CommonStates::Impl> CommonStates::Impl::instancePool;

CommonStates::Impl::Impl(_In_ ID3D12Device* device);
    : device(device)
{ 
    
}

// Helper for creating blend state objects.
HRESULT CommonStates::Impl::CreateBlendState(D3D12_BLEND srcBlend, D3D12_BLEND destBlend, _Out_ D3D12_BLEND_DESC* pResult)
{
    ZeroMemory(pResult, sizeof(D3D12_BLEND_DESC));

    pResult->RenderTarget[0].BlendEnable = (srcBlend != D3D12_BLEND_ONE) ||
                                       (destBlend != D3D12_BLEND_ZERO);

    pResult->RenderTarget[0].SrcBlend  = pResult->RenderTarget[0].SrcBlendAlpha  = srcBlend;
    pResult->RenderTarget[0].DestBlend = pResult->RenderTarget[0].DestBlendAlpha = destBlend;
    pResult->RenderTarget[0].BlendOp   = pResult->RenderTarget[0].BlendOpAlpha   = D3D12_BLEND_OP_ADD;

    pResult->RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return S_OK;
}


// Helper for creating depth stencil state objects.
HRESULT CommonStates::Impl::CreateDepthStencilState(bool enable, bool writeEnable, _Out_ D3D12_DEPTH_STENCIL_DESC* pResult)
{
    ZeroMemory(pResult, sizeof(D3D12_DEPTH_STENCIL_DESC));

    pResult->DepthEnable = enable;
    pResult->DepthWriteMask = writeEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    pResult->DepthFunc = D3D12_COMPARISON_LESS_EQUAL;

    pResult->StencilEnable = false;
    pResult->StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    pResult->StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    pResult->FrontFace.StencilFunc = D3D12_COMPARISON_ALWAYS;
    pResult->FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    pResult->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    pResult->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

    pResult->BackFace = pResult->FrontFace;

    return S_OK;
}


// Helper for creating rasterizer state objects.
HRESULT CommonStates::Impl::CreateRasterizerState(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode, _Out_ D3D12_RASTERIZER_DESC* pResult)
{
    ZeroMemory(pResult, sizeof(D3D12_RASTERIZER_DESC);

    pResult->CullMode = cullMode;
    pResult->FillMode = fillMode;
    pResult->DepthClipEnable = true;
    pResult->MultisampleEnable = true;

    return S_OK;
}

// Helper for creating sampler state objects.
HRESULT CommonStates::Impl::CreateSamplerState(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode, _Out_ D3D12_CPU_DESCRIPTOR_HANDLE pResult)
{
    D3D12_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.Filter = filter;

    desc.AddressU = addressMode;
    desc.AddressV = addressMode;
    desc.AddressW = addressMode;

    //desc.MaxAnisotropy = (device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;
    desc.MaxAnisotropy = 16;
    
    desc.MaxLOD = FLT_MAX;
    desc.ComparisonFunc = D3D12_COMPARISON_NEVER;

    HRESULT hr = device->CreateSamplerState(&desc, pResult);

    return hr;
}

D3D12_STATIC_SAMPLER_DESC CommonStates::ParseSampler(const D3D12_SAMPLER_DESC& desc)
{
    D3D12_STATIC_SAMPLER_DESC result;
    ZeroMemory(&result, sizeof(D3D12_STATIC_SAMPLER_DESC));
    
        
}

//--------------------------------------------------------------------------------------
// CommonStates
//--------------------------------------------------------------------------------------

// Public constructor.
CommonStates::CommonStates(_In_ ID3D12Device* device)
  : pImpl(Impl::instancePool.DemandCreate(device))
{
}


// Move constructor.
CommonStates::CommonStates(CommonStates&& moveFrom)
  : pImpl(std::move(moveFrom.pImpl))
{
}


// Move assignment.
CommonStates& CommonStates::operator= (CommonStates&& moveFrom)
{
    pImpl = std::move(moveFrom.pImpl);
    return *this;
}


// Public destructor.
CommonStates::~CommonStates()
{
}


//--------------------------------------------------------------------------------------
// Blend states
//--------------------------------------------------------------------------------------

D3D12_BLEND_DESC CommonStates::Opaque() const
{
    return pImpl->opaque;
}


D3D12_BLEND_DESC CommonStates::AlphaBlend() const
{
    return pImpl->alphaBlend;
}


D3D12_BLEND_DESC CommonStates::Additive() const
{
    return pImpl->additive;
}


D3D12_BLEND_DESC CommonStates::NonPremultiplied() const
{
    return pImpl->nonPremultiplied;
}


//--------------------------------------------------------------------------------------
// Depth stencil states
//--------------------------------------------------------------------------------------

D3D12_DEPTH_STENCIL_STATE CommonStates::DepthNone() const
{
    return pImpl->depthNone;
}


D3D12_DEPTH_STENCIL_STATE CommonStates::DepthDefault() const
{
    return pImpl->depthDefault;
}


D3D12_DEPTH_STENCIL_STATE CommonStates::DepthRead() const
{
    return pImpl->depthRead;
}


//--------------------------------------------------------------------------------------
// Rasterizer states
//--------------------------------------------------------------------------------------

D3D12_RASTERIZER_STATE CommonStates::CullNone() const
{
    return pImpl->cullNone;
}


D3D12_RASTERIZER_STATE CommonStates::CullClockwise() const
{
    return pImpl->cullClockwise;
}


D3D12_RASTERIZER_STATE CommonStates::CullCounterClockwise() const
{
    return pImpl->cullCounterClockwise;
}


D3D12_RASTERIZER_STATE CommonStates::Wireframe() const
{
    return pImpl->wireframe;
}


//--------------------------------------------------------------------------------------
// Sampler states
//--------------------------------------------------------------------------------------

D3D12_SAMPLER_DESC CommonStates::PointWrap() const
{
    return pImpl->pointWrap;
}


D3D12_SAMPLER_DESC CommonStates::PointClamp() const
{
    return pImpl->pointClamp;
}


D3D12_SAMPLER_DESC CommonStates::LinearWrap() const
{
    return pImpl->linearWrap;
}


D3D12_SAMPLER_DESC CommonStates::LinearClamp() const
{
    return pImpl->linearClamp;
}


D3D12_SAMPLER_DESC CommonStates::AnisotropicWrap() const
{
    return pImpl->anisotropicWrap;
}


D3D12_SAMPLER_DESC CommonStates::AnisotropicClamp() const
{
    return pImpl->anisotropicClamp;
}
