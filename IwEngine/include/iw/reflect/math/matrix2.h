#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/matrix2.h"

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::matrix2>) {
		static Class c = Class("iw::math::matrix2", sizeof(iw::math::matrix2), 1);
		c.fields[0] = {"elements", GetType(TypeTag<float [4]>()), offsetof(iw::math::matrix2, elements)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::matrix2[_s]>) {
		static Class c = Class("iw::math::matrix2""[]", sizeof(iw::math::matrix2), 1, _s);
		c.fields[0] = {"elements", GetType(TypeTag<float [4]>()), offsetof(iw::math::matrix2, elements)};
		return &c;
	}
}
}
}
