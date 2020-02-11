#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\Editor\include\Components/Enemy.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<Enemy>) {
		static Class c = Class("Enemy", sizeof(Enemy), 4);
		c.fields[0] = {"Type", GetType(TypeTag<int>()), offsetof(Enemy, Type)};
		c.fields[1] = {"Speed", GetType(TypeTag<float>()), offsetof(Enemy, Speed)};
		c.fields[2] = {"FireTime", GetType(TypeTag<float>()), offsetof(Enemy, FireTime)};
		c.fields[3] = {"CooldownTime", GetType(TypeTag<float>()), offsetof(Enemy, CooldownTime)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<Enemy[_s]>) {
		static Class c = Class("Enemy""[]", sizeof(Enemy), 4, _s);
		c.fields[0] = {"Type", GetType(TypeTag<int>()), offsetof(Enemy, Type)};
		c.fields[1] = {"Speed", GetType(TypeTag<float>()), offsetof(Enemy, Speed)};
		c.fields[2] = {"FireTime", GetType(TypeTag<float>()), offsetof(Enemy, FireTime)};
		c.fields[3] = {"CooldownTime", GetType(TypeTag<float>()), offsetof(Enemy, CooldownTime)};
		return &c;
	}
}
}
