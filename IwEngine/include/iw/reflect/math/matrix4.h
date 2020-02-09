#pragma once
#include "iw/reflection/Type.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/matrix4.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::matrix4>) {
		static Class c = Class("iw::math::matrix4", 64, 1);
		c.fields[0] = {"elements", GetType(TypeTag<float [16]>()), 0};
		return &c;
	}
}
}
