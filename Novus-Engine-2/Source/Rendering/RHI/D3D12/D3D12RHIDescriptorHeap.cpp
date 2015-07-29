#include "D3D12RHIResources.h"
#include <cassert>

namespace novus
{

HRESULT D3D12RHIDescriptorHeap::Init(ID3D12Device* device)
{
	if (ShaderVisible)
	{
		assert(DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ||
			   DescriptorHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc;

	desc.Type = DescriptorHeapType;
	desc.NumDescriptors = DescriptorCount;
	desc.Flags = (IsShaderVisible() ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	desc.NodeMask = 0;

	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(BaseHeap.ReleaseAndGetAddressOf()));

	HandleIncrementSize = device->GetDescriptorHandleIncrementSize(DescriptorHeapType);

	if (SUCCEEDED(hr))
	{
		CPUHandle = BaseHeap->GetCPUDescriptorHandleForHeapStart();
		GPUHandle = BaseHeap->GetGPUDescriptorHandleForHeapStart();
	}

	return hr;
}

void D3D12RHIDescriptorHeap::Resize(uint32_t descriptorCount)
{
	assert(BaseHeap.Get());

	ID3D12Device* device;
	BaseHeap->GetDevice(IID_PPV_ARGS(&device));

	D3D12_DESCRIPTOR_HEAP_DESC desc;

	DescriptorCount = descriptorCount;

	desc.Type = DescriptorHeapType;
	desc.NumDescriptors = descriptorCount;
	desc.Flags = (IsShaderVisible() ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	desc.NodeMask = 0;

	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(BaseHeap.ReleaseAndGetAddressOf()));

	if (SUCCEEDED(hr))
	{
		CPUHandle = BaseHeap->GetCPUDescriptorHandleForHeapStart();
		GPUHandle = BaseHeap->GetGPUDescriptorHandleForHeapStart();
	}

	assert(SUCCEEDED(hr));
}

void* D3D12RHIDescriptorHeap::GetDescriptorCPUPtr(uint32_t index) const
{
	return reinterpret_cast<void*>(CPUHandle.ptr + index * HandleIncrementSize);
}

uint64_t D3D12RHIDescriptorHeap::GetDescriptorGPUHandle(uint32_t index) const
{
	assert(ShaderVisible);

	return GPUHandle.ptr + index * HandleIncrementSize;
}

RHIDescriptorHeap& D3D12RHIDescriptorHeap::operator= (const RHIDescriptorHeap& o)
{
	//Cannot readback from shader visible descriptor heaps
	assert(DescriptorHeapType == o.GetDescriptorHeapType() && !o.IsShaderVisible());
	assert(BaseHeap.Get());

	if (DescriptorCount < o.GetDescriptorCount())
		Resize(o.GetDescriptorCount());

	ID3D12Device* device;
	BaseHeap->GetDevice(IID_PPV_ARGS(&device));

	D3D12_CPU_DESCRIPTOR_HANDLE otherHandle;
	otherHandle.ptr = reinterpret_cast<size_t>(o.GetDescriptorCPUPtr(0));

	device->CopyDescriptorsSimple(DescriptorCount, CPUHandle, otherHandle, DescriptorHeapType);

	return *this;
}

}