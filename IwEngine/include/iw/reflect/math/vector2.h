#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/vector2.h"

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::vector2>) {
		static Class c = Class("iw::math::vector2", sizeof(iw::math::vector2), 2);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::vector2, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::vector2, y)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::vector2[_s]>) {
		static Class c = Class("iw::math::vector2""[]", sizeof(iw::math::vector2), 2, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::vector2, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::vector2, y)};
		return &c;
	}
}
}
}
