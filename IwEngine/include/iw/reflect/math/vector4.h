#pragma once
#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/vector4.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::vector4>) {
		static Class c = Class("iw::math::vector4", 16, 4);
		c.fields[0] = {"x", GetType(TypeTag<float>()), 0};
		c.fields[1] = {"y", GetType(TypeTag<float>()), 4};
		c.fields[2] = {"z", GetType(TypeTag<float>()), 8};
		c.fields[3] = {"w", GetType(TypeTag<float>()), 12};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::vector4[_s]>) {
		static Class c = Class("iw::math::vector4""[]", 16, 4, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), 0};
		c.fields[1] = {"y", GetType(TypeTag<float>()), 4};
		c.fields[2] = {"z", GetType(TypeTag<float>()), 8};
		c.fields[3] = {"w", GetType(TypeTag<float>()), 12};
		return &c;
	}
}
}
