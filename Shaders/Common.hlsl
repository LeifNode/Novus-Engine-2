/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#ifndef COMMON_HLSL
#define COMMON_HLSL

#ifdef PASS_FORWARD

#define PS_OUT float4
#define PS_TARGET SV_TARGET

#define RESOLVE_SHADING(albedo, normal, specularColor, roughness, emissive)

#elif PASS_DEFERRED

#define PS_OUT float4
#define PS_TARGET

#define RESOLVE_SHADING(albedo, normal, specularColor, roughness, emissive)

#elif PASS_VOXELIZATION

#define PS_OUT float4
#define PS_TARGET

#define RESOLVE_SHADING(albedo, normal, specularColor, roughness, emissive) ()

#endif

#endif //COMMON_HLSL