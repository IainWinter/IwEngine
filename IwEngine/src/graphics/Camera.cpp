#include "iw/graphics/Camera.h"

namespace iw {
namespace Graphics {
	Camera::Camera()
	{
		RecalculateView();
	}

	Camera::Camera(
		iw::Transform* transform)
	{
		Transform.SetParent(transform);
		RecalculateView();
	}

	Camera::Camera(
		const glm::vec3& position,
		const glm::quat& rotation)
	{
		Transform.Position = position;
		Transform.Rotation = rotation;

		RecalculateView();
	}

	void Camera::SetTrans(
		iw::Transform* transform)
	{
		Transform.SetParent(transform);
	}

	void Camera::SetView(
		const glm::mat4& view)
	{
		View = view;

		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 scale;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(view, scale, rotation, translation, skew, perspective);

		Transform.Position = translation;
		Transform.Rotation = rotation;
	}

	glm::mat4 Camera::ViewProjection() {
		return Projection * View;
	}

	glm::mat4 Camera::ViewProjection() const {
		return Projection * View;
	}

	// this could cache this but there arnt that many cameras per scene so
	// its not too bad

	glm::mat4& Camera::RecalculateView() {
		//glm::vec3 wp = Transform.WorldPosition();
		//glm::quat wr = Transform.WorldRotation();

		//glm::vec3 forward = wp + wr * glm::vec3(0, 0, 1);
		//glm::vec3 up      =      wr * glm::vec3(0, 1, 0);

		//View = glm::lookAt(wp, forward, up);

		View = glm::inverse(Transform.WorldTransformation());

		return View;
	}

	OrthographicCamera::OrthographicCamera(
		float width, 
		float height, 
		float zNear, 
		float zFar
	)
		: Camera()
	{
		SetProjection(width, height, zNear, zFar);
	}

	OrthographicCamera::OrthographicCamera(
		iw::Transform* transform,
		float width, 
		float height, 
		float zNear, 
		float zFar
	)
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
		float zFar
	)
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
		Width = width;
		Height = height;
		NearClip = zNear;
		FarClip = zFar;
		RecalculateProjection();
	}

	glm::mat4& OrthographicCamera::RecalculateProjection()
	{
		// glm divs and mults by 2, this undos that
		float w  = Width    / 2.f;
		float h  = Height   / 2.f;
		float nc = NearClip * 2.f;
		float fc = FarClip  * 2.f;

		Projection = glm::ortho(-w, w, -h, h, nc, fc);
		return Projection;
	}
	
	PerspectiveCamera::PerspectiveCamera(
		float fov,
		float aspect,
		float zNear,
		float zFar
	)
		: Camera()
	{
		SetProjection(fov, aspect, zNear, zFar);
	}

	PerspectiveCamera::PerspectiveCamera(
		iw::Transform* transform,
		float fov,
		float aspect,
		float zNear,
		float zFar
	)
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
		float zFar
	)
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
		Fov = fov;
		Aspect = aspect;
		NearClip = zNear;
		FarClip = zFar;
		RecalculateProjection();
	}

	glm::mat4& PerspectiveCamera::RecalculateProjection()
	{
		Projection = glm::perspective(Fov, Aspect, NearClip, FarClip);
		return Projection;
	}
	
}
}
