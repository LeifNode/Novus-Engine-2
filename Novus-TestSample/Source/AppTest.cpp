#include "AppTest.h"
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Matrix3.h>
#include <Math/Matrix4.h>
#include <Utility/Geometry/GeometryGenerator.h>
#include <Math/Math.h>
#include <DirectXMath.h>
#include <Utility/Hashing/SHA1.h>
#include <Resources/Shader/D3D12/D3D12Shader.h>
#include <Math/Quaternion.h>
#include <Utility/Memory/MallocTracker.h>

using namespace DirectX;

#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")

#define CHK(result) \
	if (result != S_OK) \
	{					\
		throw std::runtime_error("HRESULT call failed."); \
	};

namespace novus
{

AppTest::AppTest()
	:ViewportWidth(1280),
	ViewportHeight(720),
	IndexCount(0)
{

}

AppTest::~AppTest()
{
	Destroy();
}

void AppTest::Init(HWND hwnd)
{
	LoadPipeline(hwnd);
	LoadAssets();

	Timer.Start();
}

void AppTest::Destroy()
{
	WaitForGPU();
	SwapChain->SetFullscreenState(FALSE, nullptr);

	CloseHandle(HandleEvent);

#ifdef DEBUG
	novus::MallocTracker::GetInstance()->DumpTrackedMemory();
#endif
}

void AppTest::Update()
{
	Timer.Tick();
	std::cout << Timer.GetDeltaTime() << std::endl;
}

void AppTest::Render()
{
	PopulateCommandLists();

	//Execute command lists
	std::vector<ID3D12CommandList*> commandLists;
	commandLists.push_back(CommandList.Get());

	for (auto const& it : CommandListArray)
	{
		commandLists.push_back(it.Get());
	}

	CommandQueue->ExecuteCommandLists((UINT)commandLists.size(), &commandLists[0]);

	//Swap back and front buffers
	SwapChain->Present(1, 0);
	IndexLastSwapBuf = (1 + IndexLastSwapBuf) % NumSwapBufs;
	SwapChain->GetBuffer(IndexLastSwapBuf, IID_PPV_ARGS(RenderTarget.ReleaseAndGetAddressOf()));
	Device->CreateRenderTargetView(RenderTarget.Get(), nullptr, DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	WaitForGPU();
}

void AppTest::LoadPipeline(HWND hwnd)
{
	//Create swap chain descriptor

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = NumSwapBufs;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

#ifdef DEBUG
	{
		ComPtr<ID3D12Debug> debugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT hardwareDriverSupported = CreateDeviceAndSwapChain(nullptr,
															   D3D_DRIVER_TYPE_HARDWARE,
															   D3D_FEATURE_LEVEL_11_1,
															   &swapChainDesc,
															   IID_PPV_ARGS(SwapChain.GetAddressOf()),
															   IID_PPV_ARGS(Device.GetAddressOf()),
															   IID_PPV_ARGS(CommandQueue.GetAddressOf()));

	if (FAILED(hardwareDriverSupported))
	{
		CreateDeviceAndSwapChain(nullptr,
								 D3D_DRIVER_TYPE_WARP,
								 D3D_FEATURE_LEVEL_11_1,
								 &swapChainDesc,
								 IID_PPV_ARGS(SwapChain.GetAddressOf()),
								 IID_PPV_ARGS(Device.GetAddressOf()),
								 IID_PPV_ARGS(CommandQueue.GetAddressOf()));
	}

	CHK(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

	for (unsigned int i = 0; i < ThreadCount; i++)
	{
		CHK(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocatorArray[i].GetAddressOf())));
	}
}

void AppTest::LoadAssets()
{
	//Load and compile the shaders
	D3D12Shader vertShader = D3D12Shader(ShaderType::Vertex, L"TestShader.hlsl", "VS");
	D3D12Shader pixelShader = D3D12Shader(ShaderType::Pixel, L"TestShader.hlsl", "PS");

	std::vector<ShaderMacro> macros;
	macros.push_back({ "BOXCOLOR", "float4(1.0f, 1.0f, 1.0f, 1.0f)" });

	vertShader.Compile(&macros[0], static_cast<uint32_t>(macros.size()));
	pixelShader.Compile(&macros[0], static_cast<uint32_t>(macros.size()));

	D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0}
	};

	UINT numElements = ARRAYSIZE(layout);

	//Create root signiture with one root constant buffer view

	D3D12_ROOT_PARAMETER param;
	D3D12_DESCRIPTOR_RANGE descriptorRange = {};

	if (UseRootLevelCBV)
	{
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		param.Descriptor = { 0, 0 };
	}
	else
	{
		//Used for if you need non root level constant buffer views
		//This is somewhat slower than switching out a root level CBV
		descriptorRange.BaseShaderRegister = 0;
		descriptorRange.NumDescriptors = 1;
		descriptorRange.OffsetInDescriptorsFromTableStart = 0;
		descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		descriptorRange.RegisterSpace = 0;

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &descriptorRange;
	}

	ComPtr<ID3D10Blob> pOutBlob, pErrorBlob;
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.NumParameters = 1;
	rootSigDesc.pParameters = &param;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	CHK(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf()));
	CHK(Device->CreateRootSignature(0, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), IID_PPV_ARGS(RootSignature.GetAddressOf())));

	//Create the PSO

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(psoDesc));
	psoDesc.InputLayout = { layout, numElements };
	psoDesc.pRootSignature = RootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertShader.GetByteCodePtr()), vertShader.GetSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader.GetByteCodePtr()), pixelShader.GetSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);;
	psoDesc.RasterizerState.FrontCounterClockwise = true; //Using RH coordinate system
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.StencilEnable = false;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	CHK(Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(PSO.GetAddressOf())));

	//Create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	CHK(Device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(DescriptorHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC DSVDescHeapDesc = {};
	DSVDescHeapDesc.NumDescriptors = 1;
	DSVDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	CHK(Device->CreateDescriptorHeap(&DSVDescHeapDesc, IID_PPV_ARGS(DSVDescriptorHeap.GetAddressOf())));

	CHK(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), PSO.Get(), IID_PPV_ARGS(CommandList.GetAddressOf())));

	//Create the command lists for each thread
	for (unsigned int i = 0; i < ThreadCount; i++)
	{
		CHK(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocatorArray[i].Get(), PSO.Get(), IID_PPV_ARGS(CommandListArray[i].GetAddressOf())));
		CommandListArray[i].Get()->Close();
	}

	//Create back buffer and render target

	CHK(SwapChain->GetBuffer(0, IID_PPV_ARGS(RenderTarget.GetAddressOf())));
	Device->CreateRenderTargetView(RenderTarget.Get(), nullptr, DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.Width = static_cast<float>(ViewportWidth);
	Viewport.Height = static_cast<float>(ViewportHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	RectScissor.left = 0;
	RectScissor.top = 0;
	RectScissor.right = ViewportWidth;
	RectScissor.bottom = ViewportHeight;

	//Create depth buffer
	D3D12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32_TYPELESS,
		ViewportWidth,
		ViewportHeight,
		1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_TEXTURE_LAYOUT_UNKNOWN);

	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	CHK(Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(DepthBufferTexture.GetAddressOf())));
	DepthBufferTexture->SetName(L"Depth Buffer");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	Device->CreateDepthStencilView(DepthBufferTexture.Get(), &dsvDesc, DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//Allocate buffer for all constant buffers
	PerObjectConstantBuffers.Init(BoxCount, sizeof(CBPerObject), Device.Get());

	//Create the constant buffer descriptor heap and populate it
	if (!UseRootLevelCBV)
	{
		ConstantBufferDescriptorHeap = std::make_unique<D3D12RHIDescriptorHeap>(BoxCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

		HRESULT hr = ConstantBufferDescriptorHeap->Init(Device.Get());

		assert(SUCCEEDED(hr));

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
		constantBufferViewDesc.SizeInBytes = PerObjectConstantBuffers.GetAlignedStride();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;

		for (unsigned int i = 0; i < BoxCount; i++)
		{
			cpuDescriptorHandle.ptr = reinterpret_cast<size_t>(ConstantBufferDescriptorHeap->GetDescriptorCPUPtr(i));
			constantBufferViewDesc.BufferLocation = PerObjectConstantBuffers.GetGPUHandle(i);
			Device->CreateConstantBufferView(&constantBufferViewDesc, cpuDescriptorHandle);
		}
	}

	//Generate mesh
	GeometryGenerator::SimpleMesh shapeMesh;
	
	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, shapeMesh);
	//GeometryGenerator::CreateSphere(1.0f, 10, 10, shapeMesh);

	IndexCount = static_cast<uint32_t>(shapeMesh.Indices.size());

	//Copy vertex data to buffer
	//This should be put into its own job system using the copy engine eventually
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(GeometryGenerator::SimpleVertex) * shapeMesh.Vertices.size()),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(VertBuffer.GetAddressOf()));

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint32_t) * shapeMesh.Indices.size()),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(IndexBuffer.GetAddressOf()));

	UINT8* dataBegin;
	VertBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));
	memcpy(dataBegin, &shapeMesh.Vertices[0], sizeof(GeometryGenerator::SimpleVertex) * shapeMesh.Vertices.size());
	VertBuffer->Unmap(0, nullptr);

	IndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));
	memcpy(dataBegin, &shapeMesh.Indices[0], sizeof(uint32_t) * shapeMesh.Indices.size());
	IndexBuffer->Unmap(0, nullptr);

	//Create vertex buffer view

	DescViewBufVert.BufferLocation = VertBuffer->GetGPUVirtualAddress();
	DescViewBufVert.StrideInBytes = sizeof(GeometryGenerator::SimpleVertex);
	DescViewBufVert.SizeInBytes = static_cast<UINT>(sizeof(GeometryGenerator::SimpleVertex) * shapeMesh.Vertices.size());

	DescViewBufIndex.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	DescViewBufIndex.Format = DXGI_FORMAT_R32_UINT;
	DescViewBufIndex.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * shapeMesh.Indices.size());

	//Create fencing object

	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf()));
	CurrentFence = 1;

	//Initialize bundles for drawing
	if (UseBundles)
	{
		InitBundles();
	}

	//Close the command list and use it to execute the GPU setup
	CommandList->Close();
	ID3D12CommandList* ppCommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(1, ppCommandLists);

	HandleEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

	WaitForGPU();
}

void AppTest::WaitForGPU()
{
	const UINT64 fence = CurrentFence;
	CommandQueue->Signal(Fence.Get(), fence);
	CurrentFence++;

	//Let the previous frame finish

	if (Fence->GetCompletedValue() < fence)
	{
		Fence->SetEventOnCompletion(fence, HandleEvent);
		WaitForSingleObject(HandleEvent, INFINITE);
	}
}

void AppTest::PopulateCommandLists()
{
	std::vector<std::future<void>> futures;

	for (unsigned int i = 0; i < ThreadCount - 1; i++)
	{
		auto f = std::async(&AppTest::PopulateCommandListAsync, this, i);

		futures.push_back(std::move(f));
	}

	PopulateCommandListAsync(ThreadCount - 1);

	CommandAllocator->Reset();

	CommandList->Reset(CommandAllocator.Get(), PSO.Get());

	CommandList->SetGraphicsRootSignature(RootSignature.Get());

	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &RectScissor);

	SetResourceBarrier(CommandList.Get(), RenderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };

	CommandList->ClearRenderTargetView(DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), clearColor, 0, nullptr);
	CommandList->ClearDepthStencilView(DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	SetResourceBarrier(CommandList.Get(), RenderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	CommandList->Close();

	//Wait for all tasks to finish
	for (auto & f : futures)
	{
		f.get();
	}
}

void AppTest::PopulateCommandListAsync(uint32_t threadID)
{
	CommandAllocatorArray[threadID]->Reset();

	CommandListArray[threadID]->Reset(CommandAllocatorArray[threadID].Get(), PSO.Get());

	Matrix4 view = Matrix4::LookAt(Vector3(0.0f, 6.0f, 2.0f), Vector3(), Vector3(0.0f, 1.0f, 0.0f));
	Matrix4 proj = Matrix4::Perspective(Math::PiOver4, 1280.0f / 720.0f, 0.1f, 1000.0f);

	XMMATRIX viewXM = XMMatrixLookAtRH(XMVectorSet(0.0f, 20.0f, 50.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMMATRIX projXM = XMMatrixPerspectiveFovRH(Math::PiOver4, 1280.0f / 720.0f, 1.0f, 10000.0f);

	//Update constant buffer
	CBPerObject perObject;

	void* cbUploadPtr = nullptr;

	const unsigned int start = threadID * (BoxCount / ThreadCount);
	const unsigned int end = start + (BoxCount / ThreadCount);

	//Update the constant buffer view transforms for each object
	for (unsigned int i = start; i < end; i++)
	{
		const float scale = 0.04f * static_cast<float>(i);
		const float timeOffset = 1000.0f;
		const float timeMultiplier = 0.001f;

		XMMATRIX world, invTranspose, worldView, worldViewProj;
		world = XMMatrixScaling(scale, scale, scale) * 
			XMMatrixTranslation(static_cast<float>(i), 0.0f, 0.0f) * 
			XMMatrixRotationY(-static_cast<float>((Timer.GetTotalTime() + timeOffset) * static_cast<float>(i)) * timeMultiplier) * 
			XMMatrixScaling(0.01f, 0.01f, 0.01f);

		worldView = world * viewXM;
		worldViewProj = worldView * projXM;

		memcpy_s(&perObject.World, sizeof(perObject.World), &world, sizeof(world));
		memcpy_s(&perObject.WorldViewProj, sizeof(perObject.WorldViewProj), &worldViewProj, sizeof(worldViewProj));

		//Update the constant buffer data for specified object
		cbUploadPtr = PerObjectConstantBuffers.Map(i);
		memcpy_s(cbUploadPtr, sizeof(CBPerObject), &perObject, sizeof(perObject));
		PerObjectConstantBuffers.Unmap(i);
	}

	if (!UseRootLevelCBV)
	{
		ID3D12DescriptorHeap* descriptorHeap = ConstantBufferDescriptorHeap->GetBaseHeap();
		CommandListArray[threadID]->SetDescriptorHeaps(1, &descriptorHeap);
	}

	CommandListArray[threadID]->SetGraphicsRootSignature(RootSignature.Get());

	CommandListArray[threadID]->RSSetViewports(1, &Viewport);
	CommandListArray[threadID]->RSSetScissorRects(1, &RectScissor);

	SetResourceBarrier(CommandListArray[threadID].Get(), RenderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	CommandListArray[threadID]->OMSetRenderTargets(1, &DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), true, &DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CommandListArray[threadID]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandListArray[threadID]->IASetVertexBuffers(0, 1, &DescViewBufVert);
	CommandListArray[threadID]->IASetIndexBuffer(&DescViewBufIndex);

	D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle;
	
	if (!UseBundles)
	{
		if (UseRootLevelCBV)
		{
			for (unsigned int i = start; i < end; i++)
			{
				CommandListArray[threadID]->SetGraphicsRootConstantBufferView(0, PerObjectConstantBuffers.GetGPUHandle(i));
				CommandListArray[threadID]->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
			}
		}
		else
		{
			for (unsigned int i = start; i < end; i++)
			{
				descriptorHandle.ptr = ConstantBufferDescriptorHeap->GetDescriptorGPUHandle(i);
				CommandListArray[threadID]->SetGraphicsRootDescriptorTable(0, descriptorHandle);
				CommandListArray[threadID]->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
			}
		}
	}
	else
	{
		const unsigned int bundleStart = (BundleCount / ThreadCount) * threadID;
		const unsigned int bundleEnd = bundleStart + BundleCount / ThreadCount;

		for (unsigned int i = bundleStart; i < bundleEnd; i++)
		{
			CommandListArray[threadID]->ExecuteBundle(CommandBundleArray[i].Get());
		}
	}

	SetResourceBarrier(CommandListArray[threadID].Get(), RenderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	CommandListArray[threadID]->Close();
}

void AppTest::InitBundles()
{
	CHK(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(CommandBundleAllocator.GetAddressOf())));

	for (unsigned int i = 0; i < BundleCount; i++)
	{
		const unsigned int bundlesPerThread = BundleCount / ThreadCount;
		const unsigned int threadID = i / bundlesPerThread;

		CHK(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, CommandBundleAllocator.Get(), PSO.Get(), IID_PPV_ARGS(CommandBundleArray[i].GetAddressOf())));

		ID3D12GraphicsCommandList* commandBundle = CommandBundleArray[i].Get();

		const unsigned int offset = ObjectsPerBundle * i;

		commandBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandBundle->SetGraphicsRootSignature(RootSignature.Get());

		if (UseRootLevelCBV)
		{
			for (unsigned int command = 0; command < ObjectsPerBundle; command++)
			{
				commandBundle->SetGraphicsRootConstantBufferView(0, PerObjectConstantBuffers.GetGPUHandle(offset + command));
				commandBundle->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
			}
		}
		else
		{
			D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle;

			ID3D12DescriptorHeap* descriptorHeap = ConstantBufferDescriptorHeap->GetBaseHeap();
			commandBundle->SetDescriptorHeaps(1, &descriptorHeap);

			for (unsigned int command = 0; command < ObjectsPerBundle; command++)
			{
				descriptorHandle.ptr = ConstantBufferDescriptorHeap->GetDescriptorGPUHandle(offset + command);
				commandBundle->SetGraphicsRootDescriptorTable(0, descriptorHandle);
				commandBundle->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
			}
		}

		commandBundle->Close();
	}
}

HRESULT AppTest::CreateDeviceAndSwapChain(IDXGIAdapter * adapter,
										  D3D_DRIVER_TYPE driverType, 
										  D3D_FEATURE_LEVEL minFeatureLevel, 
										  CONST DXGI_SWAP_CHAIN_DESC * swapChainDesc, 
										  REFIID riidSwapChain, 
										  void ** ppSwapChain, 
										  REFIID riidDevice, 
										  void ** ppDevice, 
										  REFIID riidQueue,
										  void ** ppQueue)
{
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory> dxgiFactory;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D12CommandQueue> queue;

	HRESULT hr = D3D12CreateDevice(adapter, minFeatureLevel, IID_PPV_ARGS(device.GetAddressOf()));

	if (FAILED(hr))
		return hr;

	D3D12_COMMAND_QUEUE_DESC queueDesc;
	ZeroMemory(&queueDesc, sizeof(queueDesc));
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(queue.GetAddressOf()));

	hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
	if (FAILED(hr))
		return hr;

	DXGI_SWAP_CHAIN_DESC localSwapChainDesc = *swapChainDesc;
	hr = dxgiFactory->CreateSwapChain(queue.Get(), &localSwapChainDesc, &swapChain);
	if (FAILED(hr))
		return hr;

	hr = device.Get()->QueryInterface(riidDevice, ppDevice);
	if (FAILED(hr))
		return hr;

	hr = queue.Get()->QueryInterface(riidQueue, ppQueue);
	if (FAILED(hr))
		return hr;

	hr = swapChain.Get()->QueryInterface(riidSwapChain, ppSwapChain);
	if (FAILED(hr))
	{
		reinterpret_cast<IUnknown*>(*ppDevice)->Release();
		return hr;
	}

	return S_OK;
}

void AppTest::SetResourceBarrier(ID3D12GraphicsCommandList * commandList, ID3D12Resource * resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = stateBefore;
	barrierDesc.Transition.StateAfter = stateAfter;

	commandList->ResourceBarrier(1, &barrierDesc);
}

};