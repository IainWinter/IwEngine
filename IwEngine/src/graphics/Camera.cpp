#include "iw/graphics/Camera.h"

namespace IW {
	Camera::Camera()
		: Position(iwm::vector3::zero)
		, Rotation(iwm::quaternion::identity) 
	{
		RecalculateView();
	}

	Camera::Camera(
		const iwm::vector3& position,
		const iwm::quaternion& rotation) 
		: Position(position)
		, Rotation(rotation)
	{
		RecalculateView();
	}

	iwm::matrix4 Camera::GetView() {
		if (Position != m_position || Rotation != m_rotation) {
			RecalculateView();
		}

		return m_view;
	}

	void Camera::SetView(
		const iwm::matrix4& view)
	{
		m_view = view;
	}

	iwm::matrix4 Camera::GetViewProjection() {
		return GetView() * GetProjection();
	}

	void Camera::RecalculateView() {
		m_view = iwm::matrix4::create_look_at(
			Position,
			Position + iwm::vector3::unit_z * Rotation,
			iwm::vector3::unit_y * Rotation);

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
		const iwm::vector3& position, 
		const iwm::quaternion& rotation, 
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
		m_projection = iwm::matrix4::create_orthographic(width, height, zNear, zFar);
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
		const iwm::vector3& position,
		const iwm::quaternion& rotation,
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
		m_projection = iwm::matrix4::create_perspective_field_of_view(fov, aspect, zNear, zFar);
	}
}
