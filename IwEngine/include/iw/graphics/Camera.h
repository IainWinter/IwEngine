#pragma once

#include "IwGraphics.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Graphics {
	struct IWGRAPHICS_API Camera {
	protected:
		iw::matrix4 m_view;
	private:
		Transform*     m_transform;
		iw::vector3    m_position;
		iw::quaternion m_rotation;
		bool m_outdated;

	public:
		Camera();

		Camera(
			Transform* transform);

		Camera(
			const vector3& position,
			const quaternion& rotation);

		//GEN_copy(, Camera)
		//GEN_move(, Camera)

		virtual ~Camera() {}

		bool Outdated() const;

		const vector3& Position() const;
		//      vector3& Position();

		const quaternion& Rotation() const;
		//      quaternion& Rotation();

		matrix4 View();
		matrix4 View() const;
		matrix4 ViewProjection();
		matrix4 ViewProjection() const;

		virtual iw::matrix4 Projection() const = 0;

		void SetTrans(
			Transform* transform);

		void SetView(
			const iw::matrix4& view);

		void SetPosition(
			const vector3& rotation);

		void SetRotation(
			const quaternion& rotation);

		virtual void SetProjection(
			const iw::matrix4& projection) = 0;
	private:
		void RecalculateView();
	};

	// Could probly do this better with strategy camera but this works for now cus there're are only 2 as far as I know

	struct IWGRAPHICS_API OrthographicCamera
		: Camera
	{
	private:
		iw::matrix4 m_projection;

	public:
		OrthographicCamera() = default;

		OrthographicCamera(
			float width,
			float height,
			float zNear,
			float zFar);

		OrthographicCamera(
			Transform* transform,
			float width,
			float height,
			float zNear,
			float zFar);

		OrthographicCamera(
			const vector3& position,
			const quaternion& rotation,
			float width,
			float height,
			float zNear,
			float zFar);

		void SetProjection(
			float width,
			float height,
			float zNear,
			float zFar);

		inline void SetProjection(
			const iw::matrix4& projection) override
		{
			m_projection = projection;
		}

		inline iw::matrix4 Projection() const override {
			return m_projection;
		}
	};

	struct IWGRAPHICS_API PerspectiveCamera
		: Camera
	{
	public:
		/*float Fov; // something like this could be cool
		float Aspect;*/
	private:
		iw::matrix4 m_projection;
		//float m_fov;
		//float m_aspect;

	public:
		PerspectiveCamera() = default;

		PerspectiveCamera(
			float fov,
			float aspect,
			float zNear,
			float zFar);

		PerspectiveCamera(
			Transform* transform,
			float fov,
			float aspect,
			float zNear,
			float zFar);

		PerspectiveCamera(
			const vector3& position,
			const quaternion& rotation,
			float fov,
			float aspect,
			float zNear,
			float zFar);

		void SetProjection(
			float fov,
			float aspect,
			float zNear,
			float zFar);

		inline void SetProjection(
			const iw::matrix4& projection) override
		{
			m_projection = projection;
		}

		inline iw::matrix4 Projection() const override {
			return m_projection;
		}
	};
}

	using namespace Graphics;
}
