#pragma once

#include <d3d12.h>
#include <stdint.h>
#include <wrl.h>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace novus
{

class D3D12BufferPool
{
public:
	D3D12BufferPool();

	~D3D12BufferPool();

	HRESULT Init(uint32_t slotCount, uint32_t bufferSize, ID3D12Device* device);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUHandle(uint32_t slot);

	void* Map(uint32_t slot);
	void Unmap(uint32_t slot);

	uint32_t GetBufferSize() const { return BufferSize; }
	uint32_t GetAlignedStride() const { return AlignedStride; }
	uint32_t GetSlotCount() const { return SlotCount; }
	uint64_t GetPoolSize() const { return PoolSize; }

private:
	ComPtr<ID3D12Resource> Resource;

	void* MappedResourcePtr;

	uint32_t BufferSize;
	uint32_t AlignedStride;

	uint32_t SlotCount;
	uint64_t PoolSize;
};

}