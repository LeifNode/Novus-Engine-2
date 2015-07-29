#pragma once

#include "Resources/Shader/Shader.h"
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace novus
{

class D3D12Shader : public ShaderBase
{
public:
	D3D12Shader(ShaderType type, const std::wstring& path, const std::string& entryPoint)
		:ShaderBase(type, path, entryPoint)
	{}

	ShaderAPI GetShaderAPI() const override { return ShaderAPI::D3D12; }
	void* GetByteCodePtr() const override { return CompiledShaderBlob.Get()->GetBufferPointer(); }

	bool Compile(const ShaderMacro* macroArr = nullptr, uint32_t macroCount = 0) override;

private:
	ComPtr<ID3DBlob> CompiledShaderBlob;
};

}