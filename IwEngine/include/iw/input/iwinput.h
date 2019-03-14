#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWINPUT_API __declspec(dllexport)
#else
#		define IWINPUT_API __declspec(dllimport)
#	endif
#endif

#ifdef IW_PLATFORM_WINDOWS
namespace IwInput {
	enum DeviceType {
		KEYBOARD,
		MOUSE
	};

	enum InputName {
		MOUSE_L_BUTTON,
		MOUSE_R_BUTTON,
		MOUSE_X_AXIS,
		MOUSE_Y_AXIS
	};
}
#endif
