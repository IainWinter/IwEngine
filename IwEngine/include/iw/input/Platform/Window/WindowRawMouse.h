#pragma once

#include "iw/input/Devices/Mouse.h"
#include "iw/util/set/sparse_set.h"

namespace IwInput {
	class WindowRawMouse
		: public Mouse
	{
	public:
	private:
		using Translation = iwutil::sparse_set<unsigned int, unsigned int>;

		Translation m_translation;

	public:
		WindowRawMouse();

		void HandleEvent(
			OsEvent& event);
	private:
	};
}
