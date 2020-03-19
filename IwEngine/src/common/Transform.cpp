#include "iw/common/Components/Transform.h"
#include "iw/log/logger.h"

namespace iw {
namespace Engine {
	Transform::Transform() 
		: Position(vector3::zero)
		, Scale(vector3::one)
		, Rotation(quaternion::identity)
		, m_parent(nullptr)
	{}

	Transform::Transform(
		vector3 position, 
		vector3 scale, 
		quaternion rotation)
		: Position(position)
		, Scale(scale)
		, Rotation(rotation)
		, m_parent(nullptr)
	{}

	matrix4 Transform::Transformation() const {
		matrix4 parent = matrix4::identity;

		if (m_parent) {
			parent = m_parent->Transformation();
		}

		return matrix4::create_from_quaternion(Rotation)
			 * matrix4::create_scale(Scale)
			 * matrix4::create_translation(Position)
			 * parent;
	}

	vector3 Transform::Forward() const {
		return vector3::unit_z * Rotation;
	}

	vector3 Transform::Right() const {
		return vector3::unit_x * Rotation;
	}

	vector3 Transform::Up() const {
		return vector3::unit_y * Rotation;
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
#ifdef IW_DEBUG
		if (std::find(m_children.begin(), m_children.end(), transform) != m_children.end()) {
			LOG_ERROR << "Tried to add duplicate child!";
			return;
		}

		if (!transform) {
			LOG_ERROR << "Tried to add null child";
			return;
		}
#endif

		transform->m_parent = this;
		m_children.push_back(transform);
	}

	void Transform::RemoveChild(
		Transform* transform)
	{
#ifdef IW_DEBUG
		if (std::find(m_children.begin(), m_children.end(), transform) == m_children.end()) {
			LOG_ERROR << "Tried to rmeove child that doesn't exist!";
			return;
		}

		if (!transform) {
			LOG_ERROR << "Tried to remove null child!";
			return;
		}
#endif

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
		Transform* transform)
	{
		if (m_parent) {
			m_parent->RemoveChild(this);
		}

		m_parent = transform;

		if (m_parent) {
			m_parent->AddChild(this);
		}
	}
}
}
