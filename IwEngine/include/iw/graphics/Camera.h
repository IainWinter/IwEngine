#pragma once

#include "IwGraphics.h"
#include "iw/math/matrix4.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Camera {
	public:
		iwm::vector3    Position;
		iwm::quaternion Rotation;
	protected:
		iwm::matrix4 m_view;
	private:
		iwm::vector3    m_position;
		iwm::quaternion m_rotation;

	public:
		Camera();

		Camera(
			const iwm::vector3& position,
			const iwm::quaternion& rotation);

		virtual ~Camera() {}

		iwm::matrix4 GetView();
		iwm::matrix4 GetViewProjection();

		virtual iwm::matrix4 GetProjection() const = 0;
	private:
		void RecalculateView();
	};

	// Could probly do this better with strategy camera but this works for now cus there're are only 2 as far as I know

	class IWGRAPHICS_API OrthographicCamera
		: public Camera
	{
	private:
		iwm::matrix4 m_projection;

	public:
		OrthographicCamera(
			float width,
			float height,
			float zNear,
			float zFar);

		OrthographicCamera(
			const iwm::vector3& position,
			const iwm::quaternion& rotation,
			float width,
			float height,
			float zNear,
			float zFar);

		void SetProjection(
			float width,
			float height,
			float zNear,
			float zFar);

		inline iwm::matrix4 GetProjection() const override {
			return m_projection;
		}
	};

	class IWGRAPHICS_API PerspectiveCamera
		: public Camera
	{
	private:
		iwm::matrix4 m_projection;

	public:
		PerspectiveCamera(
			float fov,
			float aspect,
			float zNear,
			float zFar);

		PerspectiveCamera(
			const iwm::vector3& position,
			const iwm::quaternion& rotation,
			float fov,
			float aspect,
			float zNear,
			float zFar);

		void SetProjection(
			float fov,
			float aspect,
			float zNear,
			float zFar);

		inline iwm::matrix4 GetProjection() const override {
			return m_projection;
		}
	};
}
}
