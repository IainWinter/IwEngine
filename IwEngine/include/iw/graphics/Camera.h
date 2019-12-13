#pragma once

#include "IwGraphics.h"
#include "iw/math/matrix4.h"

namespace IW {
namespace Graphics {
	class IWGRAPHICS_API Camera {
	public:
		iw::vector3    Position;
		iw::quaternion Rotation;
	protected:
		iw::matrix4 m_view;
	private:
		iw::vector3    m_position;
		iw::quaternion m_rotation;

	public:
		Camera();

		Camera(
			const iw::vector3& position,
			const iw::quaternion& rotation);

		virtual ~Camera() {}

		void SetView(
			const iw::matrix4& view);

		virtual void SetProjection(
			const iw::matrix4& projection) = 0;

		iw::matrix4 GetView();
		iw::matrix4 GetViewProjection();

		virtual iw::matrix4 GetProjection() const = 0;
	private:
		void RecalculateView();
	};

	// Could probly do this better with strategy camera but this works for now cus there're are only 2 as far as I know

	class IWGRAPHICS_API OrthographicCamera
		: public Camera
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
			const iw::vector3& position,
			const iw::quaternion& rotation,
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

		inline iw::matrix4 GetProjection() const override {
			return m_projection;
		}
	};

	class IWGRAPHICS_API PerspectiveCamera
		: public Camera
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
			const iw::vector3& position,
			const iw::quaternion& rotation,
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

		inline iw::matrix4 GetProjection() const override {
			return m_projection;
		}
	};
}

	using namespace Graphics;
}
