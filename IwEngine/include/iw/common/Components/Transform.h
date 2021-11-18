#pragma once

#include "iw/common/iwcommon.h"

namespace iw {
namespace Engine {
	struct REFLECT Transform {
	public:
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::quat Rotation;

	private:
		NO_REFLECT std::vector<Transform*> m_children;
		NO_REFLECT Transform* m_parent;

		NO_REFLECT glm::mat4 t_transformation;      // state for cache
		NO_REFLECT glm::mat4 t_worldTransformation;
		NO_REFLECT glm::vec3 t_position; 
		NO_REFLECT glm::vec3 t_scale;
		NO_REFLECT glm::quat t_rotation;
		NO_REFLECT Transform* t_parent;

	public:
		IWCOMMON_API
		Transform(
			glm::vec3 position = glm::vec3(0.f),
			glm::vec3 scale    = glm::vec3(1.f),
			glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f));

		IWCOMMON_API
		Transform(
			glm::vec2 position,
			glm::vec2 scale = glm::vec2(1),
			float rotation = 0.0f);

		IWCOMMON_API
		static Transform FromMatrix(
			glm::mat4 transformation);

		IWCOMMON_API
		bool TransformationIsOld() const;

		IWCOMMON_API
		glm::mat4 CalcTransformation() const;

		IWCOMMON_API glm::mat4 Transformation();
		IWCOMMON_API glm::vec3 Forward()        const;
		IWCOMMON_API glm::vec3 Right()          const;
		IWCOMMON_API glm::vec3 Up()             const;

		IWCOMMON_API
		bool WorldTransformationIsOld() const;

		IWCOMMON_API
		glm::mat4 CalcWorldTransformation() const;

		IWCOMMON_API glm::mat4 WorldTransformation();
		IWCOMMON_API glm::vec3 WorldForward()        const;
		IWCOMMON_API glm::vec3 WorldRight()          const;
		IWCOMMON_API glm::vec3 WorldUp()             const;
		IWCOMMON_API glm::vec3 WorldPosition()       const;
		IWCOMMON_API glm::vec3 WorldScale()          const;
		IWCOMMON_API glm::quat WorldRotation()       const;

		// returns a transform with the Scale = WorldScale and Rotation = WorldRotation
		// new transform has no parent
		IWCOMMON_API Transform ScaleAndRotation() const;

		IWCOMMON_API
		void Apply(
			Transform& transform);

		IWCOMMON_API
		size_t ChildCount() const;

		IWCOMMON_API       std::vector<Transform*>& Children();
		IWCOMMON_API const std::vector<Transform*>& Children() const;

		IWCOMMON_API void AddChild   (Transform* transform);
		IWCOMMON_API void RemoveChild(Transform* transform);

		IWCOMMON_API       Transform* Parent();
		IWCOMMON_API const Transform* Parent() const;

		IWCOMMON_API
		void SetParent(
			Transform* transform,
			bool cleanTree = true);

		IWCOMMON_API bool operator==(const Transform& other);
		IWCOMMON_API bool operator!=(const Transform& other);
	};
}

	using namespace Engine;
}
