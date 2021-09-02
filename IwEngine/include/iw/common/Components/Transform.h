#pragma once

#include "iw/common/iwcommon.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"

#include "glm/gtx/matrix_decompose.hpp" // should wrap the glm::decomp into a better signature

#include <vector>

namespace iw {
namespace Engine {
	struct REFLECT Transform {
	public:
		REFLECT glm::vec3 Position;
		REFLECT glm::vec3 Scale;
		REFLECT glm::quat Rotation;

	private:
		std::vector<Transform*> m_children;
		Transform* m_parent;

		glm::mat4 t_transformation;      // state for cache
		glm::mat4 t_worldTransformation;
		glm::vec3 t_position; 
		glm::vec3 t_scale;
		glm::quat t_rotation;
		Transform* t_parent;

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

		IWCOMMON_API void Apply(
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
