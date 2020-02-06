#pragma once
#include "iw/reflection/Type.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/vector4.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::vector4>) {
		static Class c = Class("iw::math::vector4", 16, 4);
		c.fields[0] = {"x", GetType<float>(), 0};
		c.fields[1] = {"y", GetType<float>(), 4};
		c.fields[2] = {"z", GetType<float>(), 8};
		c.fields[3] = {"w", GetType<float>(), 12};
		return &c;
	}
}
}
