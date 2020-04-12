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

	/*Camera::Camera(
		const Camera& copy)
		: m_transform(copy.m_transform), nullptr, copy.m_shadowShader, copy.m_shadowTarget)
		, m_radius(copy.m_radius)
		, m_outdated(true)
	{
		UpdateCamera();
	}

	Camera::Camera(
		Camera&& copy) noexcept
		: Light(m_intensity, copy.m_shadowCamera, copy.m_shadowShader, copy.m_shadowTarget)
		, m_radius(copy.m_radius)
		, m_outdated(true)
	{
		copy.m_shadowCamera = nullptr;
		copy.m_shadowShader = nullptr;
		copy.m_shadowTarget = nullptr;
	}

	Camera& Camera::operator=(
		const Camera& copy)
	{
		m_intensity = copy.m_intensity;
		m_shadowCamera = nullptr;
		m_shadowShader = copy.m_shadowShader;
		m_shadowTarget = copy.m_shadowTarget;
		m_radius = copy.m_radius;
		m_outdated = true;

		UpdateCamera();

		return *this;
	}

	Camera& Camera::operator=(
		Camera&& copy) noexcept
	{
		m_intensity = copy.m_intensity;
		m_shadowCamera = copy.m_shadowCamera;
		m_shadowShader = copy.m_shadowShader;
		m_shadowTarget = copy.m_shadowTarget;
		m_radius = copy.m_radius;
		m_outdated = true;

		copy.m_shadowCamera = nullptr;
		copy.m_shadowShader = nullptr;
		copy.m_shadowTarget = nullptr;

		return *this;
	}*/

	matrix4 Camera::View() {
		if (Outdated()) {
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
		const matrix4& view)
	{
		m_view = view;

		SetPosition(m_view.translation());
		SetRotation(m_view.rotation());

		m_outdated = false;
	}

	bool Camera::Outdated() const {
		return m_outdated
			|| Position() != m_position
			|| Rotation() != m_rotation;
	}

	const vector3& Camera::Position() const {
		return m_transform ? m_transform->Position : m_position;
	}

	const quaternion& Camera::Rotation() const {
		return m_transform ? m_transform->Rotation : m_rotation;
	}

	//vector3& Camera::Position() {
	//	return m_transform ? m_transform->Position : m_position;
	//}

	//quaternion& Camera::Rotation() {
	//	return m_transform ? m_transform->Rotation : m_rotation;
	//}

	void Camera::SetPosition(
		const vector3& position)
	{
		if (Position() != position) {
			if (m_transform) {
				m_transform->Position = position;
			}
		
			else {
				m_position = position;
			}

			m_outdated = true;
		}
	}

	void Camera::SetRotation(
		const quaternion& rotation)
	{
		if (Rotation() != rotation) {
			if (m_transform) {
				m_transform->Rotation = rotation;
			}

			else {
				m_rotation = rotation;
			}
	
			m_outdated = true;
		}
	}

	matrix4 Camera::ViewProjection() {
		return View() * Projection();
	}

	void Camera::RecalculateView() {
		m_view = matrix4::create_look_at(
			Position(),
			Position() + vector3::unit_z * Rotation(),
			vector3::unit_y * Rotation());


		m_position = Position();
		m_rotation = Rotation();

		m_outdated = false;
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
		m_projection = matrix4::create_orthographic(width, height, zNear, zFar);
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
		m_projection = matrix4::create_perspective_field_of_view(fov, aspect, zNear, zFar);
	}
}
}
