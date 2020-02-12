#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\Editor\include\Components/Level.h"

namespace iw {
namespace detail {
	template<>
	inline const Class* GetClass(ClassTag<Level>) {
		static Class c = Class("Level", sizeof(Level), 3);
		c.fields[0] = {"Enemies", GetClass(ClassTag<std::vector<Enemy>>()), offsetof(Level, Enemies)};
		c.fields[1] = {"Positions", GetClass(ClassTag<std::vector<iw::vector2>>()), offsetof(Level, Positions)};
		c.fields[2] = {"StageName", GetClass(ClassTag<std::string>()), offsetof(Level, StageName)};
		return &c;
	}
	template<size_t _s>
	inline const Class* GetClass(ClassTag<Level[_s]>) {
		static Class c = Class("Level""[]", sizeof(Level), 3, _s);
		c.fields[0] = {"Enemies", GetClass(ClassTag<std::vector<Enemy>>()), offsetof(Level, Enemies)};
		c.fields[1] = {"Positions", GetClass(ClassTag<std::vector<iw::vector2>>()), offsetof(Level, Positions)};
		c.fields[2] = {"StageName", GetClass(ClassTag<std::string>()), offsetof(Level, StageName)};
		return &c;
	}
}
}
