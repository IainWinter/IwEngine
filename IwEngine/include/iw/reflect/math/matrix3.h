#pragma once
#include "iw/reflection/Type.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/matrix3.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::matrix3>) {
		static Class c = Class("iw::math::matrix3", 36, 1);
		c.fields[0] = {"elements", GetType(TypeTag<float [9]>()), 0};
		return &c;
	}
}
}
