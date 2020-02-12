#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/vector4.h"

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::vector4>) {
		static Class c = Class("iw::math::vector4", sizeof(iw::math::vector4), 4);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::vector4, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::vector4, y)};
		c.fields[2] = {"z", GetType(TypeTag<float>()), offsetof(iw::math::vector4, z)};
		c.fields[3] = {"w", GetType(TypeTag<float>()), offsetof(iw::math::vector4, w)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::vector4[_s]>) {
		static Class c = Class("iw::math::vector4""[]", sizeof(iw::math::vector4), 4, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::vector4, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::vector4, y)};
		c.fields[2] = {"z", GetType(TypeTag<float>()), offsetof(iw::math::vector4, z)};
		c.fields[3] = {"w", GetType(TypeTag<float>()), offsetof(iw::math::vector4, w)};
		return &c;
	}
}
}
}
