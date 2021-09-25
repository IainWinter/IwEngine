#pragma once

#include "IwGraphics.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Graphics {
	struct Camera {
		glm::mat4 View;
		glm::mat4 Projection;
		iw::Transform Transform;

		IWGRAPHICS_API
		Camera();

		IWGRAPHICS_API
		Camera(
			iw::Transform* transform);

		IWGRAPHICS_API
		Camera(
			const glm::vec3& position,
			const glm::quat& rotation);

		IWGRAPHICS_API void SetTrans(iw::Transform* transform);
		IWGRAPHICS_API void SetView(const glm::mat4& view);

		IWGRAPHICS_API glm::mat4 ViewProjection();
		IWGRAPHICS_API glm::mat4 ViewProjection() const;

		IWGRAPHICS_API
		glm::mat4& RecalculateView();

		IWGRAPHICS_API
		virtual glm::mat4& RecalculateProjection() = 0;
	};

	// Could probly do this better with strategy camera but this works for now cus there're are only 2 as far as I know

	struct OrthographicCamera
		: Camera
	{
		float Width;
		float Height;
		float NearClip;
		float FarClip;

		IWGRAPHICS_API
		OrthographicCamera(
			float width  =  2,  // should be identity, I don't think this is
			float height =  2,
			float zNear  = -1,
			float zFar   =  1);

		IWGRAPHICS_API
		OrthographicCamera(
			iw::Transform* transform,
			float width,
			float height,
			float zNear,
			float zFar);

		IWGRAPHICS_API
		OrthographicCamera(
			const glm::vec3& position,
			const glm::quat& rotation,
			float width,
			float height,
			float zNear,
			float zFar);

		IWGRAPHICS_API
		void SetProjection(
			float width,
			float height,
			float zNear,
			float zFar);

		IWGRAPHICS_API
		glm::mat4& RecalculateProjection() override;
	};

	struct PerspectiveCamera
		: Camera
	{
		float Fov, Aspect, NearClip, FarClip;

		IWGRAPHICS_API
		PerspectiveCamera(
			float fov    = glm::radians(90.f),
			float aspect = 16.0f / 9,
			float zNear  = 0.01f,
			float zFar   = 1000.f);

		IWGRAPHICS_API
		PerspectiveCamera(
			iw::Transform* transform,
			float fov,
			float aspect,
			float zNear,
			float zFar);

		IWGRAPHICS_API
		PerspectiveCamera(
			const glm::vec3& position,
			const glm::quat& rotation,
			float fov,
			float aspect,
			float zNear,
			float zFar);

		IWGRAPHICS_API
		void SetProjection(
			float fov,
			float aspect,
			float zNear,
			float zFar);

		IWGRAPHICS_API
		glm::mat4& RecalculateProjection() override;
	};
}

	using namespace Graphics;
}
