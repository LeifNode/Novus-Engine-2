#include "D3D12BufferPool.h"
#include <cassert>

#define CHK(result) \
	if (result != S_OK) \
	{					\
		throw std::runtime_error("HRESULT call failed."); \
	};

namespace novus
{

D3D12BufferPool::D3D12BufferPool()
	:AlignedStride(0),
	SlotCount(0),
	PoolSize(0)
{
}

D3D12BufferPool::~D3D12BufferPool()
{
	if (Resource.Get())
	{
		Resource->Unmap(0, nullptr);
	}
}

HRESULT D3D12BufferPool::Init(uint32_t slotCount, uint32_t bufferSize, ID3D12Device * device)
{
	assert(bufferSize > 0);

	if (Resource.Get())
	{
		Resource->Unmap(0, nullptr);
	}

	BufferSize = bufferSize;

	//Align memory allocations in the buffer to 256 bytes
	AlignedStride = ((static_cast<uint32_t>(BufferSize) + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1));

	SlotCount = slotCount;

	PoolSize = AlignedStride * slotCount;

	D3D12_HEAP_PROPERTIES properties = {};
	properties.Type = D3D12_HEAP_TYPE_UPLOAD;
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = PoolSize;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT hr = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(Resource.ReleaseAndGetAddressOf()));

	if (SUCCEEDED(hr))
	{
		CHK(Resource->Map(0, nullptr, &MappedResourcePtr));
	}

	return hr;
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12BufferPool::GetGPUHandle(uint32_t slot)
{
	assert(slot < SlotCount);

	return Resource->GetGPUVirtualAddress() + slot * AlignedStride;
}

void * D3D12BufferPool::Map(uint32_t slot)
{
	assert(slot < SlotCount);

	return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(MappedResourcePtr) + slot * AlignedStride);
}

void D3D12BufferPool::Unmap(uint32_t slot)
{
	assert(slot < SlotCount);
}

}