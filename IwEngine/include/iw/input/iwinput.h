#pragma once

#pragma warning(suppress: 4251)

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWINPUT_API __declspec(dllexport)
#	else
#		define IWINPUT_API __declspec(dllimport)
#	endif
#endif

#ifdef IW_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

#ifdef IW_PLATFORM_WINDOWS
	typedef HWND IWI_HANDLE;
#	ifdef IWI_RAW
#		define IWI_WINRAW 1
#	else
#		define IWI_WINRAW 0
#	endif
#else
	typedef void* IWI_HANDLE;
#endif

#define iwinput iwi

#include "input_ids.h"

typedef unsigned int uint;

namespace iwinput {
	struct state_change_event;
	struct smoothed_state_change_event;

	enum device_type {
		KEYBOARD,
		MOUSE,
		//GAMEPAD
	};
}
