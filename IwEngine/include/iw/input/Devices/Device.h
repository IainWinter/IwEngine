#pragma once

#include "iw/input/IwInput.h"
#include "iw/input/DeviceInput.h"
#include "iw/common/Events/OSEvents.h"
#include "iw/util/set/tofrom_set.h"

namespace IW {
inline namespace Input {
	using Translation = iw::tofrom_set<unsigned int, InputName>;

	class IWINPUT_API Device {
	public:
		std::string Name;

	public:
		Device(
			std::string name)
			: Name(name)
		{}

		virtual ~Device() {}

		virtual DeviceInput TranslateOsEvent(
			const OsEvent& e) = 0;
	};
}
}
