#include "iw/input/Devices/Keyboard.h"

namespace iw {
namespace Input {
	Translation KeyboardBase::translation = CreateTranslation();

	unsigned int KeyboardBase::Translate(
		InputName key)
	{
		return translation.from(key);
	}

	InputName KeyboardBase::Translate(
		unsigned oskey)
	{
		return translation.to(oskey);
	}

	Translation KeyboardBase::CreateTranslation() {
		Translation translation;

#ifdef IW_PLATFORM_WINDOWS
		translation.insert('0', InputName::ZERO);
		translation.insert('1', InputName::ONE);
		translation.insert('2', InputName::TWO);
		translation.insert('3', InputName::THREE);
		translation.insert('4', InputName::FOUR);
		translation.insert('5', InputName::FIVE);
		translation.insert('6', InputName::SIX);
		translation.insert('7', InputName::SEVEN);
		translation.insert('8', InputName::EIGHT);
		translation.insert('9', InputName::NINE);
		translation.insert('A', InputName::A);
		translation.insert('B', InputName::B);
		translation.insert('C', InputName::C);
		translation.insert('D', InputName::D);
		translation.insert('E', InputName::E);
		translation.insert('F', InputName::F);
		translation.insert('G', InputName::G);
		translation.insert('H', InputName::H);
		translation.insert('I', InputName::I);
		translation.insert('J', InputName::J);
		translation.insert('K', InputName::K);
		translation.insert('L', InputName::L);
		translation.insert('M', InputName::M);
		translation.insert('N', InputName::N);
		translation.insert('O', InputName::O);
		translation.insert('P', InputName::P);
		translation.insert('Q', InputName::Q);
		translation.insert('R', InputName::R);
		translation.insert('S', InputName::S);
		translation.insert('T', InputName::T);
		translation.insert('U', InputName::U);
		translation.insert('V', InputName::V);
		translation.insert('W', InputName::W);
		translation.insert('X', InputName::X);
		translation.insert('Y', InputName::Y);
		translation.insert('Z', InputName::Z);

		translation.insert(VK_NUMPAD0,  InputName::NUM0);
		translation.insert(VK_NUMPAD1,  InputName::NUM1);
		translation.insert(VK_NUMPAD2,  InputName::NUM2);
		translation.insert(VK_NUMPAD3,  InputName::NUM3);
		translation.insert(VK_NUMPAD4,  InputName::NUM4);
		translation.insert(VK_NUMPAD5,  InputName::NUM5);
		translation.insert(VK_NUMPAD6,  InputName::NUM6);
		translation.insert(VK_NUMPAD7,  InputName::NUM7);
		translation.insert(VK_NUMPAD8,  InputName::NUM8);
		translation.insert(VK_NUMPAD9,  InputName::NUM9);
		translation.insert(VK_ADD,      InputName::ADD);
		translation.insert(VK_SUBTRACT, InputName::SUBTRACT);
		translation.insert(VK_MULTIPLY, InputName::MULTIPLY);
		translation.insert(VK_DIVIDE,   InputName::DIVIDE);
		translation.insert(VK_DECIMAL,  InputName::DECI);

		translation.insert(VK_OEM_PLUS,   InputName::PLUS);
		translation.insert(VK_OEM_MINUS,  InputName::MINUS);
		translation.insert(VK_OEM_COMMA,  InputName::COMMA);
		translation.insert(VK_OEM_PERIOD, InputName::PERIOD);

		translation.insert(VK_OEM_1, InputName::OEM1);
		translation.insert(VK_OEM_2, InputName::OEM2);
		translation.insert(VK_OEM_3, InputName::OEM3);
		translation.insert(VK_OEM_4, InputName::OEM4);
		translation.insert(VK_OEM_5, InputName::OEM5);
		translation.insert(VK_OEM_6, InputName::OEM6);
		translation.insert(VK_OEM_7, InputName::OEM7);

		translation.insert(VK_SPACE, InputName::SPACE);

		translation.insert(VK_UP,    InputName::UP);
		translation.insert(VK_LEFT,  InputName::LEFT);
		translation.insert(VK_DOWN,  InputName::DOWN);
		translation.insert(VK_RIGHT, InputName::RIGHT);

		translation.insert(VK_F1,  InputName::F1);
		translation.insert(VK_F2,  InputName::F2);
		translation.insert(VK_F3,  InputName::F3);
		translation.insert(VK_F4,  InputName::F4);
		translation.insert(VK_F5,  InputName::F5);
		translation.insert(VK_F6,  InputName::F6);
		translation.insert(VK_F7,  InputName::F7);
		translation.insert(VK_F8,  InputName::F8);
		translation.insert(VK_F9,  InputName::F9);
		translation.insert(VK_F10, InputName::F10);
		translation.insert(VK_F11, InputName::F11);
		translation.insert(VK_F12, InputName::F12);
		translation.insert(VK_F13, InputName::F13);
		translation.insert(VK_F14, InputName::F14);
		translation.insert(VK_F15, InputName::F15);
		translation.insert(VK_F16, InputName::F16);
		translation.insert(VK_F17, InputName::F17);
		translation.insert(VK_F18, InputName::F18);
		translation.insert(VK_F19, InputName::F19);
		translation.insert(VK_F20, InputName::F20);
		translation.insert(VK_F21, InputName::F21);
		translation.insert(VK_F22, InputName::F22);
		translation.insert(VK_F23, InputName::F23);
		translation.insert(VK_F24, InputName::F24);

		translation.insert(VK_BACK,   InputName::BACK);
		translation.insert(VK_TAB,    InputName::TAB);
		translation.insert(VK_CLEAR,  InputName::CLEAR);
		translation.insert(VK_RETURN, InputName::RETURN);
		translation.insert(VK_ESCAPE, InputName::ESCAPE);

		translation.insert(VK_SELECT, InputName::SELECT);
		translation.insert(VK_PRINT,  InputName::PRINT);

		translation.insert(VK_SNAPSHOT, InputName::SNAPSHOT);
		translation.insert(VK_INSERT,   InputName::INSERT);
		translation.insert(VK_DELETE,   InputName::DEL); // Windows defines DELETE as a macro!
		translation.insert(VK_PRIOR,    InputName::PRIOR);
		translation.insert(VK_NEXT,     InputName::NEXT);
		translation.insert(VK_HOME,     InputName::HOME);
		translation.insert(VK_END,      InputName::END);
		translation.insert(VK_HELP,     InputName::HELP);
		translation.insert(VK_PAUSE,    InputName::PAUSE);

		translation.insert(VK_APPS, InputName::APPS);

		translation.insert(VK_CAPITAL, InputName::CAPS_LOCK);
		translation.insert(VK_NUMLOCK, InputName::NUM_LOCK);
		translation.insert(VK_SCROLL,  InputName::SCROLL_LOCK);

		translation.insert(VK_SHIFT,    InputName::SHIFT);
		translation.insert(VK_LSHIFT,   InputName::LSHIFT);
		translation.insert(VK_RSHIFT,   InputName::RSHIFT);
		translation.insert(VK_CONTROL,  InputName::CONTROL);
		translation.insert(VK_LCONTROL, InputName::LCONTROL);
		translation.insert(VK_RCONTROL, InputName::RCONTROL);
		translation.insert(VK_MENU,     InputName::MENU);
		translation.insert(VK_LMENU,    InputName::LMENU);
		translation.insert(VK_RMENU,    InputName::RMENU);

		translation.insert(VK_LWIN, InputName::LOS);
		translation.insert(VK_RWIN, InputName::ROS);

		translation.insert(VK_CANCEL,     InputName::CANCEL);
		translation.insert(VK_CONVERT,    InputName::CONVERT);
		translation.insert(VK_NONCONVERT, InputName::NONCONVERT);
		translation.insert(VK_ACCEPT,     InputName::ACCEPT);
		translation.insert(VK_MODECHANGE, InputName::MODECHANGE);
		translation.insert(VK_EXECUTE,    InputName::EXECUTE);

		translation.insert(VK_BROWSER_BACK,      InputName::BROWSER_BACK);
		translation.insert(VK_BROWSER_FORWARD,   InputName::BROWSER_FORWARD);
		translation.insert(VK_BROWSER_REFRESH,   InputName::BROWSER_REFRESH);
		translation.insert(VK_BROWSER_STOP,      InputName::BROWSER_STOP);
		translation.insert(VK_BROWSER_SEARCH,    InputName::BROWSER_SEARCH);
		translation.insert(VK_BROWSER_FAVORITES, InputName::BROWSER_FAVORITES);
		translation.insert(VK_BROWSER_HOME,      InputName::BROWSER_HOME);

		translation.insert(VK_VOLUME_MUTE, InputName::VOLUME_MUTE);
		translation.insert(VK_VOLUME_DOWN, InputName::VOLUME_DOWN);
		translation.insert(VK_VOLUME_UP,   InputName::VOLUME_UP);

		translation.insert(VK_MEDIA_NEXT_TRACK, InputName::MEDIA_NEXT_TRACK);
		translation.insert(VK_MEDIA_PREV_TRACK, InputName::MEDIA_PREV_TRACK);
		translation.insert(VK_MEDIA_STOP,       InputName::MEDIA_STOP);
		translation.insert(VK_MEDIA_PLAY_PAUSE, InputName::MEDIA_PLAY_PAUSE);

		translation.insert(VK_LAUNCH_MAIL,         InputName::LAUNCH_MAIL);
		translation.insert(VK_LAUNCH_MEDIA_SELECT, InputName::LAUNCH_MEDIA_SELECT);
		translation.insert(VK_LAUNCH_APP1,         InputName::LAUNCH_APP1);
		translation.insert(VK_LAUNCH_APP2,         InputName::LAUNCH_APP2);

		translation.insert(VK_OEM_102,    InputName::OEM102);
		translation.insert(VK_OEM_CLEAR,  InputName::OEM_CLEAR);
		translation.insert(VK_PROCESSKEY, InputName::PROCESSKEY);
		translation.insert(VK_PACKET,     InputName::PACKET);

		translation.insert(VK_ATTN,  InputName::ATTN);
		translation.insert(VK_CRSEL, InputName::CRSEL);
		translation.insert(VK_EXSEL, InputName::EXSEL);
		translation.insert(VK_EREOF, InputName::EREOF);
		translation.insert(VK_PLAY,  InputName::PLAY);
		translation.insert(VK_ZOOM,  InputName::ZOOM);
		translation.insert(VK_PA1,   InputName::PA1);

		//translation.insert(VK_KANA, KANA);
		//translation.insert(VK_HANGEUL, HANGUEL);
		//translation.insert(VK_HANGUL, HANGUL);
		//translation.insert(VK_JUNJA, JUNJA);
		//translation.insert(VK_FINAL, FINAL);
		//translation.insert(VK_HANJA, HANJA);
		//translation.insert(VK_KANJI, KANJI);
#endif																					

		return translation;
	}

}
}
