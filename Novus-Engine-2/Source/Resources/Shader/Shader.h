/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include <string>
#include <chrono>
#include "Utility/Hashing/SHA1.h"

namespace novus
{

static const wchar_t* ShaderPathPrefix = L"../Shaders/";

enum class ShaderType
{
	Vertex,
	Geometry,
	Hull,
	Domain,
	Pixel,
	Compute
};

enum class ShaderAPI
{
	D3D11,
	D3D12,
	GL4,
	GLES3,
	Vulkan,
};

struct ShaderMacro
{
	std::string Name;
	std::string Definition;
};

class ShaderBase
{
public:
	ShaderBase(ShaderType type, const std::wstring& path, const std::string& entryPoint)
		:Type(type),
		ShaderPath(ShaderPathPrefix + path),
		EntryPoint(entryPoint),
		CompiledSize(0)
	{}

	virtual ~ShaderBase() {}

	ShaderType GetType() const { return Type; }
	SHA1Hash GetHash() const { return InputMacroHash; }
	std::wstring GetPath() const { return ShaderPath; }
	std::string GetEntryPoint() const { return EntryPoint; }
	size_t GetSize() const { return CompiledSize; }

	virtual void* GetByteCodePtr() const = 0;
	virtual ShaderAPI GetShaderAPI() const = 0;

	std::chrono::steady_clock::time_point GetLastCompileTime() const { return LastCompileTime; }

	virtual bool Compile(const ShaderMacro* macroArr = nullptr, uint32_t macroCount = 0) = 0;

	/**
	 *	Checks if the source file for the shader was modified after the last compile time.
	 */
	bool IsOutdated() const;

	static SHA1Hash HashMacros(const ShaderMacro* macroArr, uint32_t macroCount);

private:
	ShaderType Type;

protected:
	SHA1Hash InputMacroHash;
	std::wstring ShaderPath;
	std::string EntryPoint;

	size_t CompiledSize;

	std::chrono::steady_clock::time_point LastCompileTime;
};

}