//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

//Adapted from code accompanying the book Introduction to 3D Game Programming with DirectX 11 code written by Frank Luna

#include <vector>
#include <list>
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace novus
{

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector3 Tangent;
		Vector2 TexCoord;

		Vertex()
		{}

		Vertex(const Vector3& position, const Vector3& normal)
			:Position(position),
			Normal(normal)
		{}

		Vertex(const Vector3& position, const Vector3& normal, const Vector2& texCoord)
			:Position(position),
			Normal(normal),
			TexCoord(texCoord)
		{}

		Vertex(const Vector3& position, const Vector3& normal, const Vector2& texCoord, const Vector3& tangent)
			:Position(position),
			Normal(normal),
			TexCoord(texCoord),
			Tangent(tangent)
		{}

		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			Tangent(tx, ty, tz), TexCoord(u, v)
		{}
	};

	struct SimpleVertex
	{
		SimpleVertex() {}
		SimpleVertex(const Vector3& p, const Vector3& n)
			:Position(p), Normal(n) {}
		SimpleVertex(
			float px, float py, float pz,
			float nx, float ny, float nz)
			: Position(px, py, pz), Normal(nx, ny, nz) {}

		Vector3 Position;
		Vector3 Normal;
	};

	struct Mesh
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	struct SimpleMesh
	{
		std::vector<SimpleVertex> Vertices;
		std::vector<uint32_t> Indices;
	};

public:
	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	static void CreateBox(float width, float height, float depth, Mesh& mesh);

	static void CreateBox(float width, float height, float depth, SimpleMesh& mesh);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	static void CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount, Mesh& mesh);

	static void CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount, SimpleMesh& mesh);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	static void CreateGeosphere(float radius, uint32_t numSubdivisions, Mesh& mesh);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	/// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	static void CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& mesh);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	static void CreatePlane(float width, float depth, uint32_t m, uint32_t n, Mesh& mesh);

	static void CreateSkybox(std::vector<Vector3>& vertices, std::vector<uint32_t>& indices);

private:
	static void Subdivide(Mesh& meshData);
	static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& mesh);
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& mesh);
};

}