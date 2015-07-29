#pragma once

#define WIN32_LEAN_AND_MEAN

#include <d3d12.h>
#include <D3Dcompiler.h>
#include "d3dx12.h"
#include <d3d12sdklayers.h>
#include <Windows.h>
#include <wrl.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <future>
#include <atomic>
#include <Utility/Profiling/Timer.h>
#include <functional>
#include <Math/Matrix3.h>
#include <Math/Matrix4.h>
#include <Utility/Graphics/D3D12BufferPool.h>
#include <Rendering/RHI/D3D12/D3D12RHIResources.h>

using Microsoft::WRL::ComPtr;

namespace novus
{

class AppTest
{
struct CBPerObject
{
	Matrix4 World;
	Matrix4 WorldView;
	Matrix4 WorldViewProj;
	Matrix3 WorldInvTranspose;
};

struct CBPerInstance
{
	Vector3 Position;
	float pad;
	Vector3 Scale;
	float pad2;
};

public:
	AppTest();
	~AppTest();

	void Init(HWND hwnd);
	void Destroy();
	void Update();
	void Render();

private:
	void WaitForGPU();

	/*
	*   Creates the device, swap chain, and command allocator.
	*   @param hwnd The WINAPI window handle to attach the back buffer to.
	*/
	void LoadPipeline(HWND hwnd);
	void LoadAssets();


private:
	void PopulateCommandLists();

	HRESULT CreateDeviceAndSwapChain(_In_opt_ IDXGIAdapter* adapter,
		D3D_DRIVER_TYPE driverType,
		D3D_FEATURE_LEVEL minFeatureLevel,
		_In_opt_ CONST DXGI_SWAP_CHAIN_DESC* swapChainDesc,
		_In_ REFIID riidSwapChain,
		_COM_Outptr_opt_ void** ppSwapChain,
		_In_ REFIID riidDevice,
		_COM_Outptr_opt_ void** ppDevice,
		_In_ REFIID riidQueue,
		_COM_Outptr_opt_ void** ppQueue);

	void SetResourceBarrier(ID3D12GraphicsCommandList* commandList,
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter);

	/**
	 *	Populates the constant buffers for all draw calls required by the owning thread and builds the command list for the thread
	 */
	void PopulateCommandListAsync(uint32_t threadID);

	void InitBundles();

private:
	//Use bundles for batching setting the CBV and drawing each object
	static const bool UseBundles = false;
	//Use a root level CBV on the root signature or use a CBV descriptor table
	static const bool UseRootLevelCBV = true;

	static const unsigned int ThreadCount = 8;
	static const int BoxCount = 120000;
	static const int ObjectsPerBundle = 200;
	static const unsigned int BundleCount = BoxCount / ObjectsPerBundle;

	std::array<ComPtr<ID3D12CommandAllocator>, ThreadCount> CommandAllocatorArray;
	ComPtr<ID3D12CommandAllocator> CommandBundleAllocator;
	std::array<ComPtr<ID3D12GraphicsCommandList>, ThreadCount> CommandListArray;
	std::array<ComPtr<ID3D12GraphicsCommandList>, BundleCount> CommandBundleArray;

	int ViewportWidth, ViewportHeight;

	novus::Timer Timer;

	int IndexLastSwapBuf = 0;
	const int NumSwapBufs = 2;
	ComPtr<IDXGISwapChain> SwapChain;

	D3D12_VIEWPORT Viewport;
	D3D12_RECT RectScissor;

	ComPtr<ID3D12Device> Device;
	ComPtr<ID3D12Resource> RenderTarget;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> DSVDescriptorHeap;

	ComPtr<ID3D12Fence> Fence;
	UINT64 CurrentFence;
	HANDLE HandleEvent;

	ComPtr<ID3D12PipelineState> PSO;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12Resource> VertBuffer;
	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> DepthBufferTexture;

	D3D12BufferPool PerObjectConstantBuffers;

	std::unique_ptr<D3D12RHIDescriptorHeap> ConstantBufferDescriptorHeap;

	D3D12_VERTEX_BUFFER_VIEW DescViewBufVert;
	D3D12_INDEX_BUFFER_VIEW DescViewBufIndex;

	uint32_t IndexCount;
};

}