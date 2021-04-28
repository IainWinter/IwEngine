#include "iw/graphics/Camera.h"

namespace iw {
namespace Graphics {
	Camera::Camera()
		: m_transform(nullptr)
		, m_position()
		, m_rotation(1.f, 0, 0, 0)
	{
		RecalculateView();
	}

	Camera::Camera(
		Transform* transform)
		: m_transform(transform)
		, m_position()
		, m_rotation(1.f, 0, 0, 0)
	{
		RecalculateView();
	}

	Camera::Camera(
		const glm::vec3& position,
		const glm::quat& rotation)
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

	glm::mat4 Camera::View() {
		if (Outdated()) {
			RecalculateView();
		}

		return m_view;
	}

	glm::mat4 Camera::View() const {
		return m_view;
	}

	void Camera::SetTrans(
		Transform* transform)
	{
		m_transform = transform;
		m_outdated = true;
	}

	void Camera::SetView(
		const glm::mat4& view)
	{
		m_view = view;

		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 scale;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_view, scale, rotation, translation, skew, perspective);

		SetPosition(translation);
		SetRotation(rotation);

		m_outdated = false;
	}

	bool Camera::Outdated() const {
		return m_outdated
			|| Position() != m_position
			|| Rotation() != m_rotation;
	}

	glm::vec3 Camera::Position() const {
		return m_transform ? m_transform->Position : m_position;
	}

	glm::vec3 Camera::WorldPosition() const {
		return m_transform ? m_transform->WorldPosition() : m_position;
	}

	glm::quat Camera::Rotation() const {
		return m_transform ? m_transform->Rotation : m_rotation;
	}

	glm::quat Camera::WorldRotation() const {
		return m_transform ? m_transform->WorldRotation() : m_rotation;
	}

	//vector3& Camera::Position() {
	//	return m_transform ? m_transform->Position : m_position;
	//}

	//glm::quat& Camera::Rotation() {
	//	return m_transform ? m_transform->Rotation : m_rotation;
	//}

	void Camera::SetPosition(
		const glm::vec3& position)
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
		const glm::quat& rotation)
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

	glm::mat4 Camera::ViewProjection() {
		return Projection() * View();
	}

	glm::mat4 Camera::ViewProjection() const {
		return Projection() * View();
	}

	void Camera::RecalculateView() {
		glm::vec3 pos = WorldPosition();
		glm::vec3 forward = pos + glm::vec3(0, 0, 1) * WorldRotation();
		glm::vec3 up = glm::vec3(0, 1, 0) * WorldRotation();

		m_view = glm::lookAt(pos, forward, up);

		m_position = WorldPosition();
		m_rotation = WorldRotation();

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
		const glm::vec3& position,
		const glm::quat& rotation,
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
		m_projection = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, zNear, zFar);
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
		const glm::vec3& position,
		const glm::quat& rotation,
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
		m_projection = glm::perspective(fov, aspect, zNear, zFar);
	}
}
}
