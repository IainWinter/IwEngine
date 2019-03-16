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
		MOUSE,
		KEYBOARD
	};

	enum InputName : unsigned int {
		MOUSE_L_BUTTON,
		MOUSE_R_BUTTON,
		MOUSE_M_BUTTON,
		MOUSE_X1_BUTTON,
		MOUSE_X2_BUTTON,
		MOUSE_X_AXIS,
		MOUSE_Y_AXIS,
		MOUSE_WHEEL,

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

		INPUT_COUNT
	};
}
#endif
