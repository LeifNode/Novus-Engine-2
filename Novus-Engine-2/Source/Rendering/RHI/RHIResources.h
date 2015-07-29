#pragma once

/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#include <stdint.h>
#include <string>
#include <d3d12.h>

namespace novus
{

class RHIResource
{
public:
	RHIResource()
		:ResourceSize(0)
	{}

	RHIResource(uint64_t resourceSize)
		:ResourceSize(resourceSize)
	{}

	virtual ~RHIResource() 
	{}

	uint64_t GetResourceSize() const { return ResourceSize; }

	virtual void SetName(const std::wstring& name) { Name = name; }
	std::wstring GetName() const { return Name; }

protected:
	uint64_t ResourceSize;

	std::wstring Name;
};

class RHIResourceHeap : public RHIResource
{
public:
	RHIResourceHeap(uint64_t heapSize)
		:RHIResource(heapSize),
		AllocatedSpace(0)
	{}

	uint64_t GetAllocatedSpace() const { return AllocatedSpace; }

	/**
	 *	Allocates a block of GPU address space in the heap and returns the address
	 *	@param size The size of the block to allocate
	 *	@param alignment The alignment for the block of data
	 *	@returns A void pointer to the GPU virtual address space or nullptr if space could not be allocated
	 */
	virtual void* Allocate(uint64_t size, uint32_t alignment) = 0;

	/**
	 *	Clear the resource heap
	 */
	virtual void Clear() = 0;

protected:
	uint64_t AllocatedSpace;
};

class RHIDescriptorHeap : public RHIResource
{
public:
	RHIDescriptorHeap(uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE descriptorType, bool isShaderVisible = false)
		:DescriptorCount(descriptorCount),
		DescriptorHeapType(descriptorType),
		ShaderVisible(isShaderVisible)
	{}

	virtual void Resize(uint32_t descriptorCount) = 0;
	virtual void* GetDescriptorCPUPtr(uint32_t index) const = 0;
	virtual uint64_t GetDescriptorGPUHandle(uint32_t index) const = 0;

	virtual RHIDescriptorHeap& operator=(const RHIDescriptorHeap& o) = 0;

	uint32_t GetDescriptorCount() const { return DescriptorCount; }
	D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType() const { return DescriptorHeapType; }
	bool IsShaderVisible() const { return ShaderVisible; }

protected:
	uint32_t DescriptorCount;
	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType;
	bool ShaderVisible;
};

class RHIResourceView
{
public:
	RHIResourceView(RHIResource* resource)
		:Resource(resource)
	{}

	virtual ~RHIResourceView();

	RHIResource* GetResource() const { return Resource; }

protected:
	RHIResource* Resource;
};

class RHITexture : public RHIResource
{
public:
	RHITexture(DXGI_FORMAT colFormat, uint32_t mipCount, uint32_t sampleCount, uint32_t flags)
		:ColorFormat(colFormat),
		MipmapCount(mipCount),
		SampleCount(sampleCount),
		Flags(flags)
	{}

	DXGI_FORMAT GetColorFormat() const { return ColorFormat; }

	uint32_t GetSampleCount() const { return SampleCount; }

	uint32_t GetMipmapCount() const { return MipmapCount; }

private:
	DXGI_FORMAT ColorFormat;//TODO: Replace with generic enum instead of DX12 specific enum

	uint32_t SampleCount;

	uint32_t MipmapCount;

	uint32_t Flags;
};

class RHITexture2D : public RHITexture
{
public:
	RHITexture2D(uint32_t sizeX, uint32_t sizeY, DXGI_FORMAT colorFormat, uint32_t mipCount, uint32_t sampleCount, uint32_t flags)
		:RHITexture(colorFormat, mipCount, sampleCount, flags),
		SizeX(sizeX),
		SizeY(sizeY)
	{}

	uint32_t GetSizeX() const { return SizeX; }
	uint32_t GetSizeY() const { return SizeY; }


private:
	uint32_t SizeX;
	uint32_t SizeY;
};

class RHITexture2DArray : public RHITexture
{
public:
	RHITexture2DArray(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ, DXGI_FORMAT colorFormat, uint32_t mipCount, uint32_t sampleCount, uint32_t flags)
		:RHITexture(colorFormat, mipCount, sampleCount, flags),
		SizeX(sizeX),
		SizeY(sizeY),
		SizeZ(sizeZ)
	{}

	uint32_t GetSizeX() const { return SizeX; }
	uint32_t GetSizeY() const { return SizeY; }

	/**	Get the size of the texture array. */
	uint32_t GetSizeZ() const { return SizeZ; }

private:
	uint32_t SizeX;
	uint32_t SizeY;
	uint32_t SizeZ;
};

class RHITexture3D : public RHITexture
{
public:
	RHITexture3D(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ, DXGI_FORMAT colorFormat, uint32_t mipCount, uint32_t sampleCount, uint32_t flags)
		:RHITexture(colorFormat, mipCount, sampleCount, flags),
		SizeX(sizeX),
		SizeY(sizeY),
		SizeZ(sizeZ)
	{}

	uint32_t GetSizeX() const { return SizeX; }
	uint32_t GetSizeY() const { return SizeY; }
	uint32_t GetSizeZ() const { return SizeZ; }

private:
	uint32_t SizeX;
	uint32_t SizeY;
	uint32_t SizeZ;
};

class RHIVertexBufferView : public RHIResourceView
{
public:
	RHIVertexBufferView(uint32_t size, uint32_t stride, uint32_t offset, RHIResource* resource)
		:RHIResourceView(resource),
		SizeInBytes(size),
		Stride(stride),
		Offset(offset)
	{}

	uint32_t GetSize() const { return SizeInBytes; }
	uint32_t GetStride() const { return Stride; }
	uint32_t GetOffset() const { return Offset; }

private:
	uint32_t SizeInBytes;
	uint32_t Stride;

	/**
	 *	Offset into the resource's address space in bytes to start the buffer view
	 */
	uint32_t Offset;
};

class RHIIndexBufferView : public RHIResourceView
{
public:
	RHIIndexBufferView(uint32_t size, uint32_t offset, DXGI_FORMAT format, RHIResource* resource)
		:RHIResourceView(resource),
		Size(size),
		Offset(offset),
		IBFormat(format)
	{}

	uint32_t GetSize() const { return Size; }
	uint32_t GetOffset() const { return Offset; }
	DXGI_FORMAT GetFormat() const { return IBFormat; }

private:
	uint32_t Size;
	uint32_t Offset;
	DXGI_FORMAT IBFormat;
};

class RHIConstantBufferView : public RHIResourceView
{
public:
	RHIConstantBufferView(uint32_t size, RHIResource* resource)
		:RHIResourceView(resource),
		Size(size)
	{}

	uint32_t GetSize() const { return Size; }

private:
	uint32_t Size;
};

class RHIRenderTargetView : public RHIResourceView
{

};

}