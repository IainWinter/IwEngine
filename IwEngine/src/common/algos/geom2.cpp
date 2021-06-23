#include "iw/common/algos/geom2.h"

namespace iw {
namespace common {
	bool IsClockwise(
		glm::vec2 a, glm::vec2 b, glm::vec2 c) {
		return cross_length(b - a, c - a) < 0; // this was backwards before idk how that worked?
	}

	bool HasPoint(
		glm::vec2 a, glm::vec2 b, glm::vec2 c,
		glm::vec2 p) 
	{
		if ((a.x - b.x) / (a.y - b.y) == (a.x - c.x) / (a.y - c.y)) {
			return true; // points are on a line
		}

		glm::vec2 v = p;
		glm::vec2 v0 = a;
		glm::vec2 v1 = b - a;
		glm::vec2 v2 = c - a;

		float ca =  (cross_length(v, v2) - cross_length(v0, v2)) / cross_length(v1, v2);
		float cb = -(cross_length(v, v1) - cross_length(v0, v1)) / cross_length(v1, v2);

		return ca > 0 && cb > 0 && ca + cb < 1;
	}

	glm::vec2 LineIntersection(
		glm::vec2 a0, glm::vec2 a1,
		glm::vec2 b0, glm::vec2 b1)
	{
		glm::vec2 a0_a1 = a1 - a0;
		glm::vec2 b0_b1 = b1 - b0;
		glm::vec2 b0_a0 = a0 - b0;

		return a0 + a0_a1
			* iw::cross_length(b0_b1, b0_a0)
			/ iw::cross_length(a0_a1, b0_b1);
	}

	std::pair<glm::vec2, bool> SegmentIntersection(
		glm::vec2 a0, glm::vec2 a1,
		glm::vec2 b0, glm::vec2 b1)
	{
		glm::vec2 a0_a1 = a1 - a0;
		glm::vec2 b0_b1 = b1 - b0;
		glm::vec2 b0_a0 = a0 - b0;

		float x = iw::cross_length(b0_b1, b0_a0)
			/ iw::cross_length(a0_a1, b0_b1);

		return { a0 + a0_a1 * x, x >= 0 && x <= 1 };
	}
}
}
