//--------------------------------------------------------------------------------------
// File: GeometricPrimitive.cpp
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
#include "Effects.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "VertexTypes.h"
#include "SharedResourcePool.h"
#include "Bezier.h"

#include <map>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace
{
	// Collection types used when generating the geometry.
	typedef std::vector<VertexPositionNormalTexture> VertexCollection;
	typedef std::vector<uint16_t> IndexCollection;
}

namespace DirectX
{
	// Internal GeometricPrimitive implementation class.
	class GeometricPrimitiveDX12
	{
	public:
		void Initialize(_In_ ID3D12GraphicsCommandList* commandList, const VertexCollection& vertices, const IndexCollection& indices);

		void XM_CALLCONV Draw(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection, FXMVECTOR color, _In_opt_ D3D12_GPU_DESCRIPTOR_HANDLE* texture, bool wireframe, _In_opt_ std::function<void()> setCustomState);

		void Draw(_In_ IEffect* effect, /*_In_ ID3D11InputLayout* inputLayout,*/ bool alpha, bool wireframe, _In_opt_ std::function<void()> setCustomState);

		//void CreateInputLayout(_In_ IEffect* effect, _Outptr_ ID3D11InputLayout** inputLayout);

	private:
		ComPtr<ID3D12Resource> mVertexBuffer;
		ComPtr<ID3D12Resource> mIndexBuffer;

		UINT mIndexCount;

		// Only one of these helpers is allocated per D3D device context, even if there are multiple GeometricPrimitive instances.
		class SharedResources
		{
		public:
			SharedResources(_In_ ID3D12GraphicsCommandList* deviceContext);

			void PrepareForRendering(bool alpha, bool wireframe);

			ComPtr<ID3D12GraphicsCommandList> deviceContext;
			std::unique_ptr<BasicEffect> effect;

			ComPtr<ID3D11InputLayout> inputLayoutTextured;
			ComPtr<ID3D11InputLayout> inputLayoutUntextured;

			std::unique_ptr<CommonStates> stateObjects;
		};


		// Per-device-context data.
		std::shared_ptr<SharedResources> mResources;

		static SharedResourcePool<ID3D11DeviceContext*, SharedResources> sharedResourcesPool;
	};
}