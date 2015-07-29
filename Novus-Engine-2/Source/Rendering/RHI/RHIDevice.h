#pragma once

#include <d3d12.h>
#include "RHIResources.h"


namespace novus
{

class RHIDevice
{
public:
	virtual RHIConstantBufferView* CreateConstantBufferView(uint32_t size, RHIResource* resource, RHIDescriptorHeap* heap = nullptr);
	virtual RHIResourceHeap* CreateHeap(uint64_t size, D3D12_HEAP_TYPE type) = 0;
};

}