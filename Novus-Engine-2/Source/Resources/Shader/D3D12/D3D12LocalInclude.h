#pragma once

#include <d3d12.h>

/**
 *	Handles opening include files from the Shader directory when they are #included in .hlsl files
 */

namespace novus
{

class D3D12LocalInclude : public ID3DInclude
{
	HRESULT __stdcall Open(D3D_INCLUDE_TYPE Includetype, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override;
	HRESULT __stdcall Close(LPCVOID pData) override;
};

}