#pragma once
#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/quaternion.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::quaternion>) {
		static Class c = Class("iw::math::quaternion", 16, 4);
		c.fields[0] = {"x", GetType(TypeTag<float>()), 0};
		c.fields[1] = {"y", GetType(TypeTag<float>()), 4};
		c.fields[2] = {"z", GetType(TypeTag<float>()), 8};
		c.fields[3] = {"w", GetType(TypeTag<float>()), 12};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::quaternion[_s]>) {
		static Class c = Class("iw::math::quaternion""[]", 16, 4, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), 0};
		c.fields[1] = {"y", GetType(TypeTag<float>()), 4};
		c.fields[2] = {"z", GetType(TypeTag<float>()), 8};
		c.fields[3] = {"w", GetType(TypeTag<float>()), 12};
		return &c;
	}
}
}
