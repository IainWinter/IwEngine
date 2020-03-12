#include "iw/graphics/Camera.h"

namespace iw {
namespace Graphics {
	Camera::Camera()
		: m_transform(nullptr)
	{
		RecalculateView();
	}

	Camera::Camera(
		Transform* transform)
		: m_transform(transform)
	{
		RecalculateView();
	}

	Camera::Camera(
		const vector3& position,
		const quaternion& rotation)
		: m_transform(nullptr)
		, m_position(position)
		, m_rotation(rotation)
	{
		RecalculateView();
	}

	iw::matrix4 Camera::View() {
		if (   m_outdated 
			|| Position() != m_position 
			|| Rotation() != m_rotation)
		{
			RecalculateView();
		}

		return m_view;
	}

	void Camera::SetTrans(
		Transform* transform)
	{
		m_transform = transform;
		m_outdated = true;
	}

	void Camera::SetView(
		const iw::matrix4& view)
	{
		m_view = view;
	}

	const vector3& Camera::Position() const {
		return m_transform ? m_transform->Position : m_position;
	}

	const quaternion& Camera::Rotation() const {
		return m_transform ? m_transform->Rotation : m_rotation;
	}

	vector3& Camera::Position() {
		return m_transform ? m_transform->Position : m_position;
	}

	quaternion& Camera::Rotation() {
		return m_transform ? m_transform->Rotation : m_rotation;
	}

	void Camera::SetPosition(
		const vector3& position)
	{
		if (m_transform) {
			m_transform->Position = position;
		}
		
		else {
			m_position= position;
		}

		m_outdated = true;
	}

	void Camera::SetRotation(
		const quaternion& rotation)
	{
		if (m_transform) {
			m_transform->Rotation = rotation;
		}

		else {
			m_rotation = rotation;
		}

		m_outdated = true;
	}

	iw::matrix4 Camera::ViewProjection() {
		return View() * Projection();
	}

	void Camera::RecalculateView() {
		m_view = iw::matrix4::create_look_at(
			Position(),
			Position() + iw::vector3::unit_z * Rotation(),
			iw::vector3::unit_y * Rotation());

		m_position = Position();
		m_rotation = Rotation();
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
		Transform* transform,
		float width, 
		float height, 
		float zNear, 
		float zFar)
		: Camera(transform)
	{
		SetProjection(width, height, zNear, zFar);
	}

	OrthographicCamera::OrthographicCamera(
		const vector3& position,
		const quaternion& rotation,
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
		Transform* transform,
		float fov,
		float aspect,
		float zNear,
		float zFar)
		: Camera(transform)
	{
		SetProjection(fov, aspect, zNear, zFar);
	}

	PerspectiveCamera::PerspectiveCamera(
		const vector3& position,
		const quaternion& rotation,
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
