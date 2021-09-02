#include "iw/common/Components/Transform.h"
#include "iw/log/logger.h"

namespace iw {
namespace Engine {
	Transform::Transform(
		glm::vec3 position,
		glm::vec3 scale,
		glm::quat rotation
	)
		: Position(position)
		, Scale(scale)
		, Rotation(rotation)
		, m_parent(nullptr)
		, t_position()
	{
		t_transformation  = CalcTransformation();
		t_worldTransformation = t_transformation;
		t_position = Position;
		t_rotation = Rotation;
		t_scale = Scale;
		t_parent = m_parent;
	}

	Transform::Transform(
		glm::vec2 position,
		glm::vec2 scale,
		float rotation
	)
		: Position(position, 0.0f)
		, Scale(scale, 1.0f)
		, Rotation(glm::angleAxis(rotation, glm::vec3(0.0f, 0.0f, 1.0f)))
		, m_parent(nullptr)
	{
		t_transformation = CalcTransformation();
		t_worldTransformation = t_transformation;
		t_position = Position;
		t_rotation = Rotation;
		t_scale = Scale;
		t_parent = m_parent;
	}

	Transform Transform::FromMatrix(
		glm::mat4 transformation)
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformation, scale, rotation, translation, skew, perspective);

		return Transform( 
			translation, 
			scale, 
			rotation
		);
	}

	bool Transform::TransformationIsOld() const
	{
		return Position != t_position
			|| Rotation != t_rotation
			|| Scale    != t_scale;
	}

	glm::mat4 Transform::CalcTransformation() const
	{
		return glm::translate(glm::mat4(1), Position)
			 * glm::scale    (glm::mat4(1), Scale)
			 * glm::toMat4(Rotation);
	}

	glm::mat4 Transform::Transformation() 
	{
		if (TransformationIsOld())
		{
			t_transformation = CalcTransformation();
			t_position = Position;
			t_rotation = Rotation;
			t_scale = Scale;
		}

		return t_transformation;
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

	bool Transform::WorldTransformationIsOld() const
	{
		return m_parent != t_parent
			|| TransformationIsOld() 
			|| (m_parent && m_parent->WorldTransformationIsOld());
	}

	glm::mat4 Transform::CalcWorldTransformation() const
	{
		glm::mat4 parent(1);

		if (m_parent) { // could calc only old transforms in chain
			parent = m_parent->CalcWorldTransformation();
		}

		return parent * CalcTransformation();
	}

	glm::mat4 Transform::WorldTransformation() 
	{
		if (WorldTransformationIsOld())
		{
			t_worldTransformation = CalcWorldTransformation();
			t_transformation = Transformation(); // this is stupid, maybe should breakout into Trans/ParentTrans?
			t_parent = m_parent;
		}

		return t_worldTransformation;
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
