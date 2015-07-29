#include "D3D12LocalInclude.h"
#include "Resources/Shader/Shader.h"
#include "Utility/Memory/Memory.h"
#include <sstream>
#include <fstream>

namespace novus
{

HRESULT D3D12LocalInclude::Open(D3D_INCLUDE_TYPE Includetype, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
{
	std::wstringstream path;
	path << ShaderPathPrefix;
	path << pFileName;

	std::ifstream include;
	include.open(path.str(), std::ios::in | std::ios::binary | std::ios::ate);

	if (!include.good())
	{
		std::stringstream message;
		message << "Could not open include file: ";
		message << path.str().c_str() << std::endl;
		OutputDebugStringA(message.str().c_str());

		return E_FAIL;
	}

	size_t includeSize = static_cast<size_t>(include.tellg());
	*pBytes = static_cast<UINT>(includeSize);
	char* includeMem = NE_NEW char[includeSize];

	include.seekg(0, std::ios::beg);
	include.read(includeMem, includeSize);
	include.close();

	*ppData = includeMem;

	return S_OK;
}

HRESULT D3D12LocalInclude::Close(LPCVOID pData)
{
	char* includeMem = (char*)pData;

	NE_DELETEARR(includeMem);

	return S_OK;
}

}