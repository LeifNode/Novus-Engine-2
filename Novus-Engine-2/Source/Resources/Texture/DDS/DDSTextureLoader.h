//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.h
//
// Functions for loading a DDS texture and creating a Direct3D 11 runtime resource for it
//
// Note these functions are useful as a light-weight runtime loader for DDS files. For
// a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#include <d3d12.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0601

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

#if defined(_MSC_VER) && (_MSC_VER<1610) && !defined(_In_reads_)
#define _In_reads_(exp)
#define _Out_writes_(exp)
#define _In_reads_bytes_(exp)
#define _In_reads_opt_(exp)
#define _Outptr_opt_
#endif

#ifndef _Use_decl_annotations_
#define _Use_decl_annotations_
#endif

namespace DirectX
{
	enum DDS_ALPHA_MODE
	{
		DDS_ALPHA_MODE_UNKNOWN = 0,
		DDS_ALPHA_MODE_STRAIGHT = 1,
		DDS_ALPHA_MODE_PREMULTIPLIED = 2,
		DDS_ALPHA_MODE_OPAQUE = 3,
		DDS_ALPHA_MODE_CUSTOM = 4,
	};

	// Standard version
	HRESULT CreateDDSTextureFromMemory(_In_ ID3D12Device* d3dDevice,
		_In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
		_In_ size_t ddsDataSize,
		_Outptr_opt_ ID3D12Resource** texture,
		_Outptr_opt_ D3D12_SHADER_RESOURCE_VIEW_DESC** textureView,
		_In_ size_t maxsize = 0,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
		);

	HRESULT CreateDDSTextureFromFile(_In_ ID3D12Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_Outptr_opt_ ID3D12Resource** texture,
		_Outptr_opt_ D3D12_SHADER_RESOURCE_VIEW_DESC** textureView,
		_In_ size_t maxsize = 0,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
		);

	HRESULT CreateDDSTextureFromFile(_In_ ID3D12Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_Outptr_opt_ ID3D12Resource** texture,
		_Outptr_opt_ D3D12_CPU_DESCRIPTOR_HANDLE** textureView,
		_In_ size_t maxsize = 0,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
		);

	// Extended version
	HRESULT CreateDDSTextureFromMemoryEx(_In_ ID3D12Device* d3dDevice,
		_In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
		_In_ size_t ddsDataSize,
		_In_ size_t maxsize,
		_In_ unsigned int bindFlags,
		_In_ unsigned int cpuAccessFlags,
		_In_ unsigned int miscFlags,
		_In_ bool forceSRGB,
		_Outptr_opt_ ID3D12Resource** texture,
		_Outptr_opt_ D3D12_SHADER_RESOURCE_VIEW_DESC** textureView,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
		);

	HRESULT CreateDDSTextureFromFileEx(_In_ ID3D12Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_In_ size_t maxsize,
		_In_ unsigned int bindFlags,
		_In_ unsigned int cpuAccessFlags,
		_In_ unsigned int miscFlags,
		_In_ bool forceSRGB,
		_Outptr_opt_ ID3D12Resource** texture,
		_Outptr_opt_ D3D12_SHADER_RESOURCE_VIEW_DESC** textureView,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
		);

	HRESULT CreateDDSTextureFromFileEx(_In_ ID3D12Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_In_ size_t maxsize,
		_In_ unsigned int bindFlags,
		_In_ unsigned int cpuAccessFlags,
		_In_ unsigned int miscFlags,
		_In_ bool forceSRGB,
		_Outptr_opt_ ID3D12Resource** texture,
		_Outptr_opt_ D3D12_SHADER_RESOURCE_VIEW_DESC** textureView,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
		);
}