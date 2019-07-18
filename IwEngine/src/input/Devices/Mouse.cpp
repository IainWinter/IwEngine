#include "iw/input/Devices/Mouse.h"

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace IwInput {
	Translation MouseBase::translation = CreateTranslation();

	InputName MouseBase::Translate(
		unsigned int oskey)
	{
		return translation.to(oskey);
	}

	unsigned int MouseBase::Translate(
		InputName key)
	{
		return translation.from(key);
	}

	Translation MouseBase::CreateTranslation() {
		Translation translation;

#ifdef IW_PLATFORM_WINDOWS
		translation.insert(VK_LBUTTON,  MOUSE_L_BUTTON);
		translation.insert(VK_RBUTTON,  MOUSE_R_BUTTON);
		translation.insert(VK_MBUTTON,  MOUSE_M_BUTTON);
		translation.insert(VK_XBUTTON1, MOUSE_X1_BUTTON);
		translation.insert(VK_XBUTTON2, MOUSE_X2_BUTTON);
#endif

		return translation;
	}

}
