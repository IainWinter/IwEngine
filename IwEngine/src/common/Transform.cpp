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

	Transform Transform::FromMatrix(
		matrix4 transformation)
	{
		return { 
			transformation.translation(), 
			transformation.scale(), 
			transformation.rotation()
		};
	}

	matrix4 Transform::Transformation() const {
		return matrix4::create_scale(Scale)
			* matrix4::create_from_quaternion(Rotation)
			* matrix4::create_translation(Position);
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

	matrix4 Transform::WorldTransformation() const {
		matrix4 parent = matrix4::identity;

		if (m_parent) {
			parent = m_parent->WorldTransformation();
		}

		return Transformation() * parent;
	}

	vector3 Transform::WorldForward() const {
		quaternion parent = quaternion::identity;

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Forward() * parent;
	}

	vector3 Transform::WorldRight() const {
		quaternion parent = quaternion::identity;

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Right() * parent;
	}

	vector3 Transform::WorldUp() const {
		quaternion parent = quaternion::identity;

		if (m_parent) {
			parent = m_parent->WorldRotation();
		}

		return Up() * parent;
	}

	vector3 Transform::WorldPosition() const {
		vector3 parent = vector3::zero;

		if (m_parent) {
			parent = m_parent->WorldPosition();
		}

		return Position + parent;
	}

	vector3 Transform::WorldScale() const {
		vector3 parent = vector3::one;

		if (m_parent) {
			parent = m_parent->WorldScale();
		}

		return Scale * parent;
	}

	quaternion Transform::WorldRotation() const {
		quaternion parent = quaternion::identity;

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
