#include "iw/graphics/Camera.h"

namespace iw {
namespace Graphics {
	Camera::Camera()
		: Position(iw::vector3::zero)
		, Rotation(iw::quaternion::identity) 
	{
		RecalculateView();
	}

	Camera::Camera(
		const iw::vector3& position,
		const iw::quaternion& rotation) 
		: Position(position)
		, Rotation(rotation)
	{
		RecalculateView();
	}

	iw::matrix4 Camera::GetView() {
		if (Position != m_position || Rotation != m_rotation) {
			RecalculateView();
		}

		return m_view;
	}

	void Camera::SetView(
		const iw::matrix4& view)
	{
		m_view = view;
	}

	iw::matrix4 Camera::GetViewProjection() {
		return GetView() * GetProjection();
	}

	void Camera::RecalculateView() {
		m_view = iw::matrix4::create_look_at(
			Position,
			Position + iw::vector3::unit_z * Rotation,
			iw::vector3::unit_y * Rotation);

		m_position = Position;
		m_rotation = Rotation;
	}

	OrthographicCamera::OrthographicCamera(
		float width, 
		float height, 
		float zNear, 
		float zFar)
	{
		SetProjection(width, height, zNear, zFar);
	}

	OrthographicCamera::OrthographicCamera(
		const iw::vector3& position, 
		const iw::quaternion& rotation, 
		float width, 
		float height, 
		float zNear, 
		float zFar)
		: Camera(position, rotation)
	{
		SetProjection(width, height, zNear, zFar);
	}

	void OrthographicCamera::SetProjection(
		float width, 
		float height, 
		float zNear, 
		float zFar)
	{
		m_projection = iw::matrix4::create_orthographic(width, height, zNear, zFar);
	}
	
	PerspectiveCamera::PerspectiveCamera(
		float fov,
		float aspect,
		float zNear,
		float zFar)
	{
		SetProjection(fov, aspect, zNear, zFar);
	}

	PerspectiveCamera::PerspectiveCamera(
		const iw::vector3& position,
		const iw::quaternion& rotation,
		float fov,
		float aspect,
		float zNear,
		float zFar)
		: Camera(position, rotation)
	{
		SetProjection(fov, aspect, zNear, zFar);
	}

	void PerspectiveCamera::SetProjection(
		float fov, 
		float aspect, 
		float zNear, 
		float zFar)
	{
		m_projection = iw::matrix4::create_perspective_field_of_view(fov, aspect, zNear, zFar);
	}
}
}
