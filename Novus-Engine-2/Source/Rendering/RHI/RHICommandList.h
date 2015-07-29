/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

/**
 *	Heavily based on the Rendering Hardware Interface pattern in UE4 by Epic Games
 *	I'm using this since I want to eventually go and build out the backend for DX11 and GL4/ES3, and it has very little overhead for abstracting the command list building
 *	Uses CRTP for static polymorphism to reduce overhead
 *	06/25/15 - Going to leave this until I am more familiar with DX12 and have an implementation using that. 
 *			   Looked at the full UE4 implementation and it will take a while to put in commands/states for all of DX12
 */

#pragma once

#include <stdint.h>
#include "Math/Vector4.h"

namespace novus
{
	class RHIResource;
	class RHITexture;
	class RHITexture2D;
	class RHIPipelineState;
}

namespace novus
{

enum class ClearFlags : uint8_t
{
	Depth = 1,
	Stencil = 1 << 1,
};

class RHICommandListBase
{
	
public:
	//virtual ~RHICommandList() {}


	//virtual void ClearState(RHIPipelineState* state);

	//virtual void Close();

	//virtual void ClearDepthStencil(RHITexture2D* depthStencil, ClearFlags clearFlags, float clearDepth, uint8_t clearStencil) = 0;
	//virtual void ClearRenderTarget(RHITexture* renderTarget, const Vector4& clearColor);

	//virtual void ClearUnorderedAccessView(RHIResource* resource, const Vector4& value);
	//virtual void ClearUnorderedAccessView(RHIResource* resource, const Vector4u& value);

	//virtual void CopyBufferRegion(RHIResource* dstBuffer, uint64_t dstOffset, RHIResource* srcBuffer, uint64_t srcOffset, uint64_t byteCount);

private:

};

struct RHICommandBase
{
	RHICommandBase* Next;

	void(*ExecuteAndDestructPtr)(RHICommandListBase& commandList, RHICommandBase* command);

	inline RHICommandBase(void(*executeAndDestructPtr)(RHICommandListBase& commandList, RHICommandBase* command))
		:Next(nullptr),
		ExecuteAndDestructPtr(executeAndDestructPtr)
	{}

	inline void CallExecuteAndDestruct(RHICommandListBase& commandList)
	{
		ExecuteAndDestructPtr(commandList, this);
	}
};

template <typename TCmd>
struct RHICommand : public RHICommandBase
{
	inline RHICommand()
		:RHICommandBase(&ExecuteAndDestruct)
	{}

	static inline void ExecuteAndDestruct(RHICommandListBase& commandList, RHICommandBase* command)
	{
		TCmd *thisCmd = static_cast<TCmd*>(command);
		thisCmd->Execute(commandList);
		thisCmd->~TCmd();
	}
};



}