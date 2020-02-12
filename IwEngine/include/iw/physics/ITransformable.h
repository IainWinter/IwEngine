#pragma once

#include "iw/common/Components/Transform.h"

namespace iw {
namespace Physics {
	template<
		typename T>
	struct ITransformable {
		virtual void Transform(
			const iw::Transform& transform) = 0;

		T Transformed(
			const iw::Transform& transform)
		{
			T t = (const T&)*this;
			t.Transform(transform);
			return t;
		}
	};
}
}
