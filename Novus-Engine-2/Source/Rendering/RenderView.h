#pragma once

#include <stdint.h>
#include <functional>
#include "RenderTarget.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

/**
 *	RenderView represents a view into the 3D scene.
 *	It contains all data for the viewport transform, rendering state, render target type, etc.
 */

namespace novus
{

namespace RenderTargetFlags
{
enum Type : uint8_t
{
	DepthOnly = 0x01,
	Opaque = 0x02,
	Transparent = 0x04,
	Static = 0x08,
	Dynamic = 0x10,
	All = Opaque | Transparent | Static | Dynamic,
	Volumetric = 0x20, //Voxelization
};
}

enum class RenderingSortOrder
{
	None,
	FrontBack,
	BackFront
};

class RenderTarget;
class Transform;
class Camera;

class RendererView
{
public:
	RendererView(RenderTarget* target, RenderingSortOrder sortOrder, uint8_t cullFlags = RenderTargetFlags::All);

	Matrix4 GetView() const { return ViewMatrix; }
	Matrix4 GetProjection() const { return ProjectionMatrix; }
	Matrix4 GetViewProjection() const;

	Vector3 GetViewPosition() const { return ViewPosition; }
	Quaternion GetViewRotation() const { return ViewRotation; }

	bool IsOrthographic() const { return bIsOrthographic; }

	bool IsEnabled() const { return bIsEnabled; }
	void SetEnabled(bool enabled) { bIsEnabled = enabled; }

	bool RequiresUpdate() const { return bRequiresUpdate; }
	void SetRequiresUpdate(bool requiresUpdate) { bRequiresUpdate = requiresUpdate; }

	uint8_t GetViewTypeFlags() const { return TypeFlags; }
	RenderingSortOrder GetSortOrder() const { return SortOrder; }

	RenderTarget* GetRenderTarget() const { return ViewRenderTarget; }

	void SetProjectionPerspective(float fovVertical, float viewportWidth, float viewportHeight, float zMin, float zMax);
	void SetProjectionOrthographic(float height, float width, float depth);
	void SetProjectionOrthographic(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);

	void SetView(const Transform& view);
	void SetView(const Vector3& position, const Quaternion& rotation);

	void SetViewProjection(const Camera& camera);

	/*void SetPredicate(const std::function<bool()>& predicate);
	void ClearPredicate();
	bool HasPredicate() const { return bHasPredicate; }*/

	uint16_t GetPriority() const { return Priority; }
	void SetPriority(uint16_t priority) { Priority = priority; }

private:
	inline void UpdateTransforms() const;

private:
	mutable Matrix4 ViewMatrix;
	Matrix4 ProjectionMatrix;
	mutable Matrix4 ViewProjectionMatrix;
	mutable bool bRequiresTransformUpdate;

	Vector3 ViewPosition;
	Quaternion ViewRotation;

	bool bIsOrthographic;
	bool bIsEnabled;
	bool bRequiresUpdate;
	bool bIsDebugView;

	uint8_t TypeFlags; //OR of different RenderTargetTypeFlags
	RenderingSortOrder SortOrder;

	RenderTarget* ViewRenderTarget;

	uint16_t Priority;

	/*std::function<bool()> RenderPredicate;
	bool bHasPredicate;*/
};

}