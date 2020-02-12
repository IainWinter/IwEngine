#pragma once
#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/vector2.h"

namespace iw {
namespace detail {
	template<>
	inline const Class* GetClass(ClassTag<iw::math::vector2>) {
		static Class c = Class("iw::math::vector2", 8, 2);
		c.fields[0] = {"x", GetType(TypeTag<float>()), 0};
		c.fields[1] = {"y", GetType(TypeTag<float>()), 4};
		return &c;
	}
	template<size_t _s>
	inline const Class* GetClass(ClassTag<iw::math::vector2[_s]>) {
		static Class c = Class("iw::math::vector2""[]", 8, 2, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), 0};
		c.fields[1] = {"y", GetType(TypeTag<float>()), 4};
		return &c;
	}
}
}
