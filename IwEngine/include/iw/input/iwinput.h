#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWINPUT_API __declspec(dllexport)
#else
#		define IWINPUT_API __declspec(dllimport)
#	endif
#endif

#define NO_WIDTH  -1.0f
#define NO_HEIGHT -1.0f

namespace IW {
namespace Input {
	enum class DeviceType
		: int
	{
		MOUSE,      RAW_MOUSE,
		KEYBOARD,   RAW_KEYBOARD,
		DEVICE_NONE
	};

	enum InputName
		: unsigned int
	{
		// Nice numbers and letters

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

		// Num pad 

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
		MULTIPLY,
		DIVIDE,
		DECI,
		//SEPARATOR, // never sent by keyboards?

		// Oem versions of these are the same everywhere I guess

		PLUS,
		MINUS,
		COMMA,
		PERIOD,

		// Oem keys like -=][';/.,`

		OEM1,
		OEM2,
		OEM3,
		OEM4,
		OEM5,
		OEM6,
		OEM7,
		OEM8,

		// Space bar

		SPACE,

		// Arrow Keys

		UP,
		LEFT,
		DOWN,
		RIGHT,

		// Function keys

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

		// Formating keys

		BACK,
		TAB,
		CLEAR,
		RETURN,
		ESCAPE,

		// Func ass keys

		SELECT,
		PRINT,

		SNAPSHOT,
		INSERT,
		DEL,
		PRIOR,
		NEXT,
		HOME,
		END,
		HELP,
		PAUSE,

		APPS,

		// Locks

		CAPS_LOCK,
		NUM_LOCK,
		SCROLL_LOCK,

		// Sys leys

		SHIFT,
		LSHIFT,
		RSHIFT,
		CONTROL,
		LCONTROL,
		RCONTROL,
		MENU,
		LMENU,
		RMENU,

		// Os keys

		LOS,
		ROS,

		// weird keys i =dk

		CANCEL,
		CONVERT,
		NONCONVERT,
		ACCEPT,
		MODECHANGE,
		EXECUTE,

		// browser keys ?

		BROWSER_BACK,
		BROWSER_FORWARD,
		BROWSER_REFRESH,
		BROWSER_STOP,
		BROWSER_SEARCH,
		BROWSER_FAVORITES,
		BROWSER_HOME,

		// Media keys

		VOLUME_MUTE,
		VOLUME_DOWN,
		VOLUME_UP,

		MEDIA_NEXT_TRACK,
		MEDIA_PREV_TRACK,
		MEDIA_STOP,
		MEDIA_PLAY_PAUSE,

		LAUNCH_MAIL,
		LAUNCH_MEDIA_SELECT,
		LAUNCH_APP1,
		LAUNCH_APP2,

		// Wak ass oem 

		OEM102,
		OEM_CLEAR,
		PROCESSKEY,
		PACKET,

		// What the fuck ass keys are these

		ATTN,
		CRSEL,
		EXSEL,
		EREOF,
		PLAY,
		ZOOM,
		//NONAME,
		PA1,

		// Other langs but these are busted and some are the same so idk

		//KANA,
		//HANGUEL,
		//HANGUL,
		//JUNJA,
		//FINAL,
		//HANJA,
		//KANJI,

		// Mouse

		LMOUSE,
		RMOUSE,
		MMOUSE,
		WHEEL,
		XMOUSE1,
		XMOUSE2,
		MOUSEX,
		MOUSEY,
		MOUSEdX,
		MOUSEdY,

		// 200iq

		INPUT_COUNT,
		INPUT_NONE
	};

	//Order of InputNames is important

	constexpr char KeyTranslation[SPACE * 2 + 2] = {
		// Numbers and letters

		'0', ')',
		'1',	'!',
		'2',	'@',
		'3',	'#',
		'4',	'$',
		'5',	'%',
		'6',	'^',
		'7',	'&',
		'8',	'*',
		'9',	'(',
		'a',	'A',
		'b',	'B',
		'c',	'C',
		'd',	'D',
		'e',	'E',
		'f',	'F',
		'g',	'G',
		'h',	'H',
		'i',	'I',
		'j',	'J',
		'k',	'K',
		'l',	'L',
		'm',	'M',
		'n',	'N',
		'o',	'O',
		'p',	'P',
		'q',	'Q',
		'r',	'R',
		's',	'S',
		't',	'T',
		'u',	'U',
		'v',	'V',
		'w',	'W',
		'x',	'X',
		'y',	'Y',
		'z',	'Z',

		// Num pad

		'0',	'\0',   
		'1',	'\0',
		'2',	'\0',
		'3',	'\0',
		'4',	'\0',
		'5',	'\0',
		'6',	'\0',
		'7',	'\0',
		'8',	'\0',
		'9',	'\0',
		'+',	'+',
		'-',	'-',
		'*',	'*',
		'/',	'/',
		'.',	'\0',

		// Oem keys

		'=',	'+',
		'-',	'_',
		',',	'<',
		'.',	'>',

		';',  ':',
		'/',	 '?',
		'`',	 '~',
		'[',	 '{',
		'\\', '|',
		']',	 '}',
		'\'', '\"',
		'\0', '\0',

		' ', ' '
	};

	static char GetCharacter(
		InputName name,
		bool shift = false,
		bool caps = false)
	{
		bool shifted = shift;
		if (name >= A && name <= Z) {
			shifted ^= caps;
		}

		if (name <= SPACE) {
			return KeyTranslation[shifted ? name * 2 + 1 : name * 2];
		}

		return '\0';
	}
}

	using namespace Input;
}
