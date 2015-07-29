#pragma once

#include "RHI/RHIResources.h"
#include <memory>

namespace novus
{

class RenderTarget
{
public:
	RenderTarget();

	~RenderTarget();

	HRESULT Init(uint32_t width, uint32_t height, DXGI_FORMAT format);

	DXGI_FORMAT GetFormat() const { return TargetResource->GetColorFormat(); }

	uint32_t GetSizeX() const { return TargetResource->GetSizeX(); }
	uint32_t GetSizeY() const { return TargetResource->GetSizeY(); }

private:
	RHITexture2D* TargetResource;
	RHIRenderTargetView* TargetView;
};

}