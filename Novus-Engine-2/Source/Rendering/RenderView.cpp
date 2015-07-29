#pragma once

#include "RenderView.h"
#include "Math/Matrix3.h"

namespace novus
{

RendererView::RendererView(RenderTarget* target, RenderingSortOrder sortOrder, uint8_t cullFlags)
	:ViewRenderTarget(target),
	SortOrder(sortOrder),
	TypeFlags(cullFlags),
	bIsOrthographic(false),
	bIsEnabled(true),
	bRequiresUpdate(true),
	bIsDebugView(false),
	Priority(0),
	ViewMatrix(1.0f),
	ProjectionMatrix(1.0f),
	ViewProjectionMatrix(1.0f),
	bRequiresTransformUpdate(true)
{}

Matrix4 RendererView::GetViewProjection() const
{
	UpdateTransforms();

	return ViewMatrix;
}

void RendererView::SetProjectionPerspective(float fovVertical, float viewportWidth, float viewportHeight, float zMin, float zMax)
{
	bRequiresTransformUpdate = true;
	bIsOrthographic = false;
	ProjectionMatrix = Matrix4::Perspective(fovVertical, viewportWidth / viewportHeight, zMin, zMax);
}

void RendererView::SetProjectionOrthographic(float height, float width, float depth)
{
	bRequiresTransformUpdate = true;
	bIsOrthographic = true;
	ProjectionMatrix = Matrix4::Orthographic(width, height, 0.0f, depth);
}

void RendererView::SetProjectionOrthographic(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ)
{
	bRequiresTransformUpdate = true;
	bIsOrthographic = true;
	ProjectionMatrix = Matrix4::OrthographicOffCenter(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ);
}

void RendererView::SetView(const Transform& view)
{
	bRequiresTransformUpdate = true;

}

void RendererView::SetView(const Vector3& position, const Quaternion& rotation)
{
	bRequiresTransformUpdate = true;
	ViewPosition = position;
	ViewRotation = rotation;

	const Matrix3 rotationMat = Matrix3(Quaternion::ToMatrix(ViewRotation));
	const Vector3 lookDirection = rotationMat * Vector3(0.0f, 0.0f, 1.0f);

	ViewMatrix = Matrix4::LookToward(ViewPosition, lookDirection, Vector3(0.0f, 1.0f, 0.0f));
}

void RendererView::SetViewProjection(const Camera& camera)
{

}

void RendererView::UpdateTransforms() const
{
	if (bRequiresTransformUpdate)
	{
		const Matrix3 rotation = Matrix3(Quaternion::ToMatrix(ViewRotation));
		const Vector3 lookDirection = rotation * Vector3(0.0f, 0.0f, 1.0f);

		ViewMatrix = Matrix4::LookToward(ViewPosition, lookDirection, Vector3(0.0f, 1.0f, 0.0f));
		ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;

		bRequiresTransformUpdate = false;
	}
}

}