/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#include "Common.hlsl"

#ifndef BOXCOLOR
#define BOXCOLOR float4(1.0f, 1.0f, 1.0f, 1.0f)
#endif

cbuffer CBPerObject : register(b0)
{
	float4x4 cWorld;
	float4x4 cWorldView;
	float4x4 cWorldViewProj;
	float3x3 cWorldInvTranspose;
};

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
};

struct PS_INPUT
{
	float4 Position  : SV_POSITION;
	float3 PositionW : POSITION;
	float3 Normal    : NORMAL;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.Position = mul(cWorldViewProj, float4(input.Position, 1.0f));
	output.PositionW = mul(cWorld, float4(input.Position, 1.0f)).xyz;
	output.Normal = mul(cWorld, float4(input.Normal, 0.0f)).xyz;

	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	const float3 lightDir = normalize(float3(-1.0f, 1.0f, 1.0f));
	const float4 color = BOXCOLOR;

	input.Normal = normalize(input.Normal);

	const float4 finalColor = float4(color.rgb * saturate(dot(input.Normal, lightDir)), color.a);

	return finalColor;
}