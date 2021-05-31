#pragma once

#include "IwGraphics.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Graphics {
	struct IWGRAPHICS_API Camera {
	protected:
		glm::mat4 m_view;
	private:
		Transform* m_transform;

		glm::vec3 m_position;
		glm::quat m_rotation;

		bool m_outdated;

	public:
		Camera();

		Camera(
			Transform* transform);

		Camera(
			const glm::vec3& position,
			const glm::quat& rotation);

		//GEN_copy(, Camera)
		//GEN_move(, Camera)

		virtual ~Camera() {}

		bool Outdated() const;

		glm::vec3 Position() const;
		glm::vec3 WorldPosition() const;

		glm::quat Rotation() const;
		glm::quat WorldRotation() const;

		glm::mat4 View();
		glm::mat4 View() const;
		glm::mat4 ViewProjection();
		glm::mat4 ViewProjection() const;

		virtual glm::mat4 Projection() const = 0;

		void SetTrans(
			Transform* transform);

		void SetView(
			const glm::mat4& view);

		void SetPosition(
			const glm::vec3& rotation);

		void SetRotation(
			const glm::quat& rotation);

		virtual void SetProjection(
			const glm::mat4& projection) = 0;
	private:
		void RecalculateView();
	};

	// Could probly do this better with strategy camera but this works for now cus there're are only 2 as far as I know

	struct IWGRAPHICS_API OrthographicCamera
		: Camera
	{
	private:
		glm::mat4 m_projection = glm::mat4();
		float m_width  =  2;
		float m_height =  2;
		float m_zNear  = -1;
		float m_zFar   =  1;

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
			const glm::vec3& position,
			const glm::quat& rotation,
			float width,
			float height,
			float zNear,
			float zFar);

		void SetProjection(
			float width,
			float height,
			float zNear,
			float zFar);

		inline float Width()    { return m_width; }
		inline float Height()   { return m_height; }
		inline float NearClip() { return m_zNear; }
		inline float FarClip()  { return m_zFar; }

		inline void Width   (float width)  { m_width = width; }
		inline void Height  (float height) { m_height = height; }
		inline void NearClip(float zNear)  { m_zNear = zNear; }
		inline void FarClip (float zFar)   { m_zFar = zFar; }

		inline void SetProjection(
			const glm::mat4& projection) override
		{
			m_projection = projection;
		}

		inline glm::mat4 Projection() const override {
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
		glm::mat4 m_projection = glm::mat4();;
		//float m_fov;
		//float m_aspect;

	public:
		PerspectiveCamera(
			float fov    = 100 * glm::pi<float>() / 180,
			float aspect = 16.0f / 9,
			float zNear  = 0.01f,
			float zFar   = 1000.0f);

		PerspectiveCamera(
			Transform* transform,
			float fov,
			float aspect,
			float zNear,
			float zFar);

		PerspectiveCamera(
			const glm::vec3& position,
			const glm::quat& rotation,
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
			const glm::mat4& projection) override
		{
			m_projection = projection;
		}

		inline glm::mat4 Projection() const override {
			return m_projection;
		}
	};
}

	using namespace Graphics;
}
