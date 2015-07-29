#include "D3D12RHIResources.h"
#include <cassert>

namespace novus
{

uint64_t Align(uint64_t location, uint64_t alignment)
{
	//Only allow power of two alignments
	assert(alignment != 0 && !(alignment & (alignment - 1)));

	return ((location + (alignment - 1)) & ~(alignment - 1));
}

D3D12RHIResourceHeap::~D3D12RHIResourceHeap()
{
	BaseResource->Unmap(0, nullptr);
}

HRESULT D3D12RHIResourceHeap::Init(uint64_t size, D3D12_HEAP_TYPE type, ID3D12Device* device)
{
	D3D12_HEAP_PROPERTIES properties = {};
	properties.Type = type;
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_HEAP_DESC desc = {};
	desc.Properties = properties;
	desc.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.SizeInBytes = size;
	
	HRESULT hr = device->CreateHeap(&desc, IID_PPV_ARGS(BaseResource.ReleaseAndGetAddressOf()));

	if (SUCCEEDED(hr))
	{
		BaseResource->Map(0, nullptr, reinterpret_cast<void**>(&MappedMemLoc));

		this->ResourceSize = size;
		this->AllocatedSpace = 0;
		this->CurrentAllocOffset = 0;
	}

	return hr;
}

void * D3D12RHIResourceHeap::Allocate(uint64_t size, uint32_t alignment)
{
	const uint64_t newOffset = Align(CurrentAllocOffset, alignment);

	assert(size + newOffset <= ResourceSize);//Check for overflow

	CurrentAllocOffset = newOffset + size;

	AllocatedSpace = CurrentAllocOffset;

	return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(MappedMemLoc) + CurrentAllocOffset);
}

void D3D12RHIResourceHeap::Clear()
{
	this->AllocatedSpace = 0;
	this->CurrentAllocOffset = 0;
}

}