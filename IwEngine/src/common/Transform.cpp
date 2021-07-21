#include "iw/common/Components/Transform.h"
#include "iw/log/logger.h"

namespace iw {
namespace Engine {
	Transform::Transform() 
		: Position(glm::vec3(0))
		, Scale(glm::vec3(1))
		, Rotation(glm::quat(1, 0, 0, 0))
		, m_parent(nullptr)
	{}

	Transform::Transform(
		glm::vec3 position,
		glm::vec3 scale,
		glm::quat rotation
	)
		: Position(position)
		, Scale(scale)
		, Rotation(rotation)
		, m_parent(nullptr)
	{}

	IWCOMMON_API
	Transform::Transform(
		glm::vec2 position,
		glm::vec2 scale,
		float rotation
	)
		: Position(position, 0.0f)
		, Scale(scale, 1.0f)
		, Rotation(glm::angleAxis(rotation, glm::vec3(0.0f, 0.0f, 1.0f)))
		, m_parent(nullptr)
	{}

	Transform Transform::FromMatrix(
		glm::mat4 transformation)
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformation, scale, rotation, translation, skew, perspective);

		return { 
			translation, 
			scale, 
			rotation
		};
	}

	glm::mat4 Transform::Transformation() const 
	{
		return glm::translate(glm::mat4(1), Position)
			 * glm::scale(glm::mat4(1), Scale)
			 * glm::toMat4(Rotation);
	}

	glm::vec3 Transform::Forward() const {
		return Rotation * glm::vec3(0, 0, 1);
	}

	glm::vec3 Transform::Right() const {
		return Rotation * glm::vec3(1, 0, 0);
	}

	glm::vec3 Transform::Up() const {
		return Rotation * glm::vec3(0, 1, 0);
	}

	glm::mat4 Transform::WorldTransformation() const {
		glm::mat4 parent(1);

		if (m_parent) {
			parent = m_parent->WorldTransformation();
		}

		return parent * Transformation();
	}

	glm::vec3 Transform::WorldForward() const {
		glm::quat parent(1, 0, 0, 0);

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Forward() * parent;
	}

	glm::vec3 Transform::WorldRight() const {
		glm::quat parent(1, 0, 0, 0);

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Right() * parent;
	}

	glm::vec3 Transform::WorldUp() const {
		glm::quat parent(1, 0, 0, 0);

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Up() * parent;
	}

	glm::vec3 Transform::WorldPosition() const {
		glm::vec3 parent(0);

		if (m_parent) {
			parent = m_parent->WorldPosition();
		}

		return Position + parent;
	}

	glm::vec3 Transform::WorldScale() const {
		glm::vec3 parent(1);

		if (m_parent) {
			parent = m_parent->WorldScale();
		}

		return Scale * parent;
	}

	glm::quat Transform::WorldRotation() const {
		glm::quat parent(1, 0, 0, 0);

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Rotation * parent;
	}

	void Transform::Apply(
		iw::Transform& transform)
	{
		Position += transform.Position;
		Rotation *= transform.Rotation;
		Scale    *= transform.Scale;
	}

	size_t Transform::ChildCount() const {
		return m_children.size();
	}

	std::vector<Transform*>& Transform::Children() {
		return m_children;
	}

	const std::vector<Transform*>& Transform::Children() const {
		return m_children;
	}

	void Transform::AddChild(
		Transform* transform)
	{
		if (std::find(m_children.begin(), m_children.end(), transform) != m_children.end()) {
			LOG_ERROR << "Tried to add duplicate child!";
			return;
		}

		if (!transform) {
			LOG_ERROR << "Tried to add null child";
			return;
		}

		transform->m_parent = this;
		m_children.push_back(transform);
	}

	void Transform::RemoveChild(
		Transform* transform)
	{
		if (std::find(m_children.begin(), m_children.end(), transform) == m_children.end()) {
			LOG_ERROR << "Tried to rmeove child that doesn't exist!";
			return;
		}

		if (!transform) {
			LOG_ERROR << "Tried to remove null child!";
			return;
		}

		transform->m_parent = nullptr;

		m_children.erase(
			std::find(m_children.begin(), m_children.end(), transform)
		);
	}

	Transform* Transform::Parent() {
		return m_parent;
	}

	const Transform* Transform::Parent() const {
		return m_parent;
	}

	void Transform::SetParent(
		Transform* transform,
		bool cleanTree)
	{
		if (m_parent == transform) {
			return;
		}

		if (cleanTree && m_parent) {
			m_parent->RemoveChild(this);
		}

		m_parent = transform;

		if (cleanTree && m_parent) {
			m_parent->AddChild(this);
		}
	}

	bool Transform::operator==(
		const Transform& other)
	{
		return WorldPosition() == other.WorldPosition()
			&& WorldScale()    == other.WorldScale()
			&& WorldRotation() == other.WorldRotation();
	}

	bool Transform::operator!=(
		const Transform& other)
	{
		return !this->operator==(other);
	}
}
}
