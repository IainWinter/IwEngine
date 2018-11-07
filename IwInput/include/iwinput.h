#pragma once

//Construct input device without event bus

#pragma warning(suppress: 4251)

#ifdef IWINPUT_EXPORT
#	define IWINPUT_API __declspec(dllexport)
#else
#	define IWINPUT_API __declspec(dllimport)
#endif

#ifdef _WIN32
#	define IWI_WINDOWS 1
#	include <Windows.h>
#else
#	define IWI_WINDOWS 0
#endif /* _WIN32 */

#ifdef IWDEBUG
#	define IWI_DEBUG 1
#else
#	define IWI_RELEASE 1
#endif /* IWDEBUG */

#ifdef IWI_WINDOWS
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

typedef unsigned int uint;

namespace iwinput {
	struct state_change_event;
	struct smoothed_state_change_event;

	enum device_type {
		KEYBOARD,
		MOUSE,
		//GAMEPAD
	};

	enum keyboard_input {
		BACKSPACE,
		TAB,
		CLEAR,
		RETURN,
		PAUSE,
		ESCAPE,
		SPACE,
		PAGEUP,
		PADEDOWN,
		END,
		HOME,
		SELECT,
		PRINT,
		EXECUTE,
		PRINTSCREEN,
		DEL,
		HELP,

		ZERO,
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,

		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		NUM0,
		NUM1,
		NUM2,
		NUM3,
		NUM4,
		NUM5,
		NUM6,
		NUM7,
		NUM8,
		NUM9,
		ADD,
		SUBTRACT,
		DIVIDE,
		MULTIPLY,
		DECIAML,
		SEPARATOR,

		LEFT,
		UP,
		RIGHT,
		DOWN,

		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,

		CAPS_LOCK,
		NUM_LOCK,
		SCROLL_LOCK,

		SHIFT,
		LEFT_SHIFT,
		RIGHT_SHIFT,
		CONTROL,
		LEFT_CONTROL,
		RIGHT_CONTROL,
		MENU,
		LEFT_MENU,
		RIGHT_MENU,

		LEFT_WIN,
		RIGHT_WIN,

		KEYBOARD_INPUT_COUNT
	};

	enum mouse_input {
		X_SPEED = KEYBOARD_INPUT_COUNT,
		Y_SPEED,
		X_POSITION,
		Y_POSITION,
		L_MOUSE,
		R_MOUSE,

		MOUSE_INPUT_COUNT
	};
}