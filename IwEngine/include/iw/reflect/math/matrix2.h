#pragma once
#include "iw/reflection/Type.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/matrix2.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::matrix2>) {
		static Class c = Class("iw::math::matrix2", 16, 1);
		c.fields[0] = {"elements", GetType(TypeTag<float [4]>()), 0};
		return &c;
	}
}
}
