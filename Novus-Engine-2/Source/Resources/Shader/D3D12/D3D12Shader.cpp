#include "D3D12Shader.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <D3Dcompiler.h>
#include <cassert>
#include "Utility/Memory/Memory.h"
#include "D3D12LocalInclude.h"

using namespace std::experimental::filesystem;

namespace novus
{

bool D3D12Shader::Compile(const ShaderMacro * macroArr, uint32_t macroCount)
{
	std::ifstream shaderFile;
	shaderFile.open(ShaderPath, std::ios::in | std::ios::binary | std::ios::ate);

	if (!shaderFile.good())
		return false;

	size_t fileSize = static_cast<size_t>(shaderFile.tellg());

	char* shaderSource = NE_NEW char[fileSize];

	shaderFile.seekg(0, std::ios::beg);
	shaderFile.read(shaderSource, fileSize);
	shaderFile.close();

	D3D_SHADER_MACRO* d3dMacroArr = nullptr;

	if (macroArr != nullptr && macroCount > 0)
	{
		d3dMacroArr = new D3D_SHADER_MACRO[macroCount + 1];
		
		//Copy macro pointers to D3D specific structure
		for (unsigned int i = 0; i < macroCount; i++)
		{
			d3dMacroArr[i].Name = macroArr[i].Name.c_str();
			d3dMacroArr[i].Definition = macroArr[i].Definition.c_str();
		}

		d3dMacroArr[macroCount].Name = nullptr;
		d3dMacroArr[macroCount].Definition = nullptr;
	}

	std::string target;

	switch (this->GetType())
	{
	case ShaderType::Vertex:
		target = "vs_5_0";
		break;
	case ShaderType::Geometry:
		target = "gs_5_0";
		break;
	case ShaderType::Hull:
		target = "hs_5_0";
		break;
	case ShaderType::Domain:
		target = "ds_5_0";
		break;
	case ShaderType::Pixel:
		target = "ps_5_0";
		break;
	case ShaderType::Compute:
		target = "cs_5_0";
		break;
	default:
		assert(false);
	}

	UINT shaderCompileFlag = 0;

#ifdef DEBUG
	shaderCompileFlag |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> errorBlob;

	//Handles including files inside shaders
	D3D12LocalInclude include;

	//Compile the shader
	HRESULT hr = D3DCompile(
		shaderSource, 
		fileSize, 
		nullptr, 
		d3dMacroArr, 
		&include, 
		EntryPoint.c_str(), 
		target.c_str(), 
		shaderCompileFlag, 
		0, 
		CompiledShaderBlob.ReleaseAndGetAddressOf(), 
		errorBlob.ReleaseAndGetAddressOf());

	NE_DELETEARR(shaderSource);
	delete[] d3dMacroArr;

	//If there are any errors print them to the debug console
	if (errorBlob.Get())
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	CompiledSize = CompiledShaderBlob->GetBufferSize();
	InputMacroHash = HashMacros(macroArr, macroCount);
	LastCompileTime = std::chrono::steady_clock::now();

	return SUCCEEDED(hr);
}

}