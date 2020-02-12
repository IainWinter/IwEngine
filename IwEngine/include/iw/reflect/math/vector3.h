#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/vector3.h"

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::vector3>) {
		static Class c = Class("iw::math::vector3", sizeof(iw::math::vector3), 3);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::vector3, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::vector3, y)};
		c.fields[2] = {"z", GetType(TypeTag<float>()), offsetof(iw::math::vector3, z)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::vector3[_s]>) {
		static Class c = Class("iw::math::vector3""[]", sizeof(iw::math::vector3), 3, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::vector3, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::vector3, y)};
		c.fields[2] = {"z", GetType(TypeTag<float>()), offsetof(iw::math::vector3, z)};
		return &c;
	}
}
}
}
