#pragma once

#include "iw/math/matrix4.h"

namespace IwEngine {
	class Transform {
	private:
		iwm::matrix4 Transformation;

	public:
		iwm::vector3& Position() {
			return Transformation.translation();
		}

	};
}
