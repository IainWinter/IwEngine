#include "iw/input/Devices/Mouse.h"

namespace iw {
namespace Input {
	Translation MouseBase::translation = CreateTranslation();

	unsigned MouseBase::Translate(
		InputName key)
	{
		return translation.from(key);
	}

	InputName MouseBase::Translate(
		unsigned oskey)
	{
		return translation.to(oskey);
	}

	Translation MouseBase::CreateTranslation() {
		Translation translation;

#ifdef IW_PLATFORM_WINDOWS
		translation.insert(VK_LBUTTON,  LMOUSE);
		translation.insert(VK_RBUTTON,  RMOUSE);
		translation.insert(VK_MBUTTON,  MMOUSE);
		translation.insert(VK_XBUTTON1, XMOUSE1);
		translation.insert(VK_XBUTTON2, XMOUSE2);
#endif

		return translation;
	}

}
}
