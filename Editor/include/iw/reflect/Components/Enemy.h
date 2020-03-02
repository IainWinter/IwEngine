#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\Editor\include\Components/Enemy.h"

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<Enemy>) {
		static Class c = Class("Enemy", sizeof(Enemy), 5);
		c.fields[0] = {"Type", GetType(TypeTag<int>()), offsetof(Enemy, Type)};
		c.fields[1] = {"Bullet", GetClass(ClassTag<struct Bullet>()), offsetof(Enemy, Bullet)};
		c.fields[2] = {"Speed", GetType(TypeTag<float>()), offsetof(Enemy, Speed)};
		c.fields[3] = {"FireTime", GetType(TypeTag<float>()), offsetof(Enemy, FireTime)};
		c.fields[4] = {"CooldownTime", GetType(TypeTag<float>()), offsetof(Enemy, CooldownTime)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<Enemy[_s]>) {
		static Class c = Class("Enemy""[]", sizeof(Enemy), 5, _s);
		c.fields[0] = {"Type", GetType(TypeTag<int>()), offsetof(Enemy, Type)};
		c.fields[1] = {"Bullet", GetClass(ClassTag<struct Bullet>()), offsetof(Enemy, Bullet)};
		c.fields[2] = {"Speed", GetType(TypeTag<float>()), offsetof(Enemy, Speed)};
		c.fields[3] = {"FireTime", GetType(TypeTag<float>()), offsetof(Enemy, FireTime)};
		c.fields[4] = {"CooldownTime", GetType(TypeTag<float>()), offsetof(Enemy, CooldownTime)};
		return &c;
	}
}
}
}
