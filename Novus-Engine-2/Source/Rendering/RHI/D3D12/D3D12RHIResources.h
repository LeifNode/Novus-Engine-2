#pragma once

#include "Rendering/RHI/RHIResources.h"
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace novus
{

class D3D12RHIResourceHeap : public RHIResourceHeap
{
	friend class D3D12RHIDevice;
private:
	/*D3D12RHIResourceHeap()
		:RHIResourceHeap(0),
		CurrentAllocOffset(0),
		MappedMemLoc(nullptr)
	{}*/

public:
	D3D12RHIResourceHeap()
		:RHIResourceHeap(0),
		CurrentAllocOffset(0),
		MappedMemLoc(nullptr)
	{}

	~D3D12RHIResourceHeap();

	void* Allocate(uint64_t size, uint32_t alignment) override;

	void Clear() override;

private:
	HRESULT Init(uint64_t size, D3D12_HEAP_TYPE type, ID3D12Device* device);

private:
	//ComPtr<ID3D12Heap> BaseResource; //Uncomment this and fix the heap allocation
	ComPtr<ID3D12Resource> BaseResource;

	uint64_t CurrentAllocOffset;

	void* MappedMemLoc;
};

class D3D12RHIDescriptorHeap : public RHIDescriptorHeap
{
public:
	D3D12RHIDescriptorHeap(uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE descriptorType, bool isShaderVisible = true)
		:RHIDescriptorHeap(descriptorCount, descriptorType, isShaderVisible),
		HandleIncrementSize(0)
	{}

	HRESULT Init(ID3D12Device* device);

	void Resize(uint32_t descriptorCount) override;
	void* GetDescriptorCPUPtr(uint32_t index) const override;
	uint64_t GetDescriptorGPUHandle(uint32_t index) const override;

	ID3D12DescriptorHeap* GetBaseHeap() const { return BaseHeap.Get(); }

	RHIDescriptorHeap& operator= (const RHIDescriptorHeap& o) override;

private:
	ComPtr<ID3D12DescriptorHeap> BaseHeap;
	uint32_t HandleIncrementSize;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
};

class D3D12RHITexture2D : public RHITexture2D
{

};


}