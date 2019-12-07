#pragma once

#include "iw/common/Components/Transform.h"

namespace IW {
namespace Physics {
	template<
		typename T>
	struct ITransformable {
		virtual void Transform(
			const IW::Transform& transform) = 0;

		T Transformed(
			const IW::Transform& transform)
		{
			T t = (const T&)*this;
			t.Transform(transform);
			return t;
		}
	};
}
}
