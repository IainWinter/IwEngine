#pragma once

#include "iw/common/Components/Transform.h"
#include "glm/vec2.hpp"

namespace iw {
namespace common {
	IWCOMMON_API
	bool IsClockwise(
		glm::vec2 a, glm::vec2 b, glm::vec2 c);

	IWCOMMON_API
	bool HasPoint(
		glm::vec2 a, glm::vec2 b, glm::vec2 c,
		glm::vec2 p);

	IWCOMMON_API
	glm::vec2 LineIntersection(
		glm::vec2 a0, glm::vec2 a1,
		glm::vec2 b0, glm::vec2 b1);

	IWCOMMON_API
	std::pair<glm::vec2, bool> SegmentIntersection(
		glm::vec2 a0, glm::vec2 a1,
		glm::vec2 b0, glm::vec2 b1);

	IWCOMMON_API
	float TriangleArea(
		const glm::vec2& a,
		const glm::vec2& b,
		const glm::vec2& c);

	template<
		Dimension _d>
	_vec<_d> TransformPoint(
		const _vec<_d>& point,
		const Transform* transform)
	{
		glm::vec4 v;

		if constexpr (_d == d2) v = glm::vec4(point, 0, 1);
		else                    v = glm::vec4(point, 1);

		return _vec<_d>(v * glm::transpose(transform->WorldTransformation()));
	}
}
	using namespace common;
}
