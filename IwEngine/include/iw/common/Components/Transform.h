#pragma once

#include "iw/common/iwcommon.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"
#include <vector>

#include "glm/gtx/matrix_decompose.hpp"

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

	public:
		IWCOMMON_API
		Transform();

		IWCOMMON_API
		Transform(
			glm::vec3 position,
			glm::vec3 scale    = glm::vec3(1),
			glm::quat rotation = glm::quat(1, 0, 0, 0));

		IWCOMMON_API
		Transform(
			glm::vec2 position,
			glm::vec2 scale = glm::vec2(1),
			float rotation = 0.0f);

		IWCOMMON_API
		static Transform FromMatrix(
			glm::mat4 transformation);

		IWCOMMON_API glm::mat4 Transformation() const;
		IWCOMMON_API glm::vec3 Forward()        const;
		IWCOMMON_API glm::vec3 Right()          const;
		IWCOMMON_API glm::vec3 Up()             const;

		IWCOMMON_API glm::mat4 WorldTransformation() const;
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
