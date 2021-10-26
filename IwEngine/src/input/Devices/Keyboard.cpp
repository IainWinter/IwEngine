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
		translation.insert('0', ZERO);
		translation.insert('1', ONE);
		translation.insert('2', TWO);
		translation.insert('3', THREE);
		translation.insert('4', FOUR);
		translation.insert('5', FIVE);
		translation.insert('6', SIX);
		translation.insert('7', SEVEN);
		translation.insert('8', EIGHT);
		translation.insert('9', NINE);
		translation.insert('A', A);
		translation.insert('B', B);
		translation.insert('C', C);
		translation.insert('D', D);
		translation.insert('E', E);
		translation.insert('F', F);
		translation.insert('G', G);
		translation.insert('H', H);
		translation.insert('I', I);
		translation.insert('J', J);
		translation.insert('K', K);
		translation.insert('L', L);
		translation.insert('M', M);
		translation.insert('N', N);
		translation.insert('O', O);
		translation.insert('P', P);
		translation.insert('Q', Q);
		translation.insert('R', R);
		translation.insert('S', S);
		translation.insert('T', T);
		translation.insert('U', U);
		translation.insert('V', V);
		translation.insert('W', W);
		translation.insert('X', X);
		translation.insert('Y', Y);
		translation.insert('Z', Z);

		translation.insert(VK_NUMPAD0, NUM0);
		translation.insert(VK_NUMPAD1, NUM1);
		translation.insert(VK_NUMPAD2, NUM2);
		translation.insert(VK_NUMPAD3, NUM3);
		translation.insert(VK_NUMPAD4, NUM4);
		translation.insert(VK_NUMPAD5, NUM5);
		translation.insert(VK_NUMPAD6, NUM6);
		translation.insert(VK_NUMPAD7, NUM7);
		translation.insert(VK_NUMPAD8, NUM8);
		translation.insert(VK_NUMPAD9, NUM9);
		translation.insert(VK_ADD, ADD);
		translation.insert(VK_SUBTRACT, SUBTRACT);
		translation.insert(VK_MULTIPLY, MULTIPLY);
		translation.insert(VK_DIVIDE, DIVIDE);
		translation.insert(VK_DECIMAL, DECI);

		translation.insert(VK_OEM_PLUS, PLUS);
		translation.insert(VK_OEM_MINUS, MINUS);
		translation.insert(VK_OEM_COMMA, COMMA);
		translation.insert(VK_OEM_PERIOD, PERIOD);

		translation.insert(VK_OEM_1, OEM1);
		translation.insert(VK_OEM_2, OEM2);
		translation.insert(VK_OEM_3, OEM3);
		translation.insert(VK_OEM_4, OEM4);
		translation.insert(VK_OEM_5, OEM5);
		translation.insert(VK_OEM_6, OEM6);
		translation.insert(VK_OEM_7, OEM7);

		translation.insert(VK_SPACE, SPACE);

		translation.insert(VK_UP, UP);
		translation.insert(VK_LEFT, LEFT);
		translation.insert(VK_DOWN, DOWN);
		translation.insert(VK_RIGHT, RIGHT);

		translation.insert(VK_F1, F1);
		translation.insert(VK_F2, F2);
		translation.insert(VK_F3, F3);
		translation.insert(VK_F4, F4);
		translation.insert(VK_F5, F5);
		translation.insert(VK_F6, F6);
		translation.insert(VK_F7, F7);
		translation.insert(VK_F8, F8);
		translation.insert(VK_F9, F9);
		translation.insert(VK_F10, F10);
		translation.insert(VK_F11, F11);
		translation.insert(VK_F12, F12);
		translation.insert(VK_F13, F13);
		translation.insert(VK_F14, F14);
		translation.insert(VK_F15, F15);
		translation.insert(VK_F16, F16);
		translation.insert(VK_F17, F17);
		translation.insert(VK_F18, F18);
		translation.insert(VK_F19, F19);
		translation.insert(VK_F20, F20);
		translation.insert(VK_F21, F21);
		translation.insert(VK_F22, F22);
		translation.insert(VK_F23, F23);
		translation.insert(VK_F24, F24);

		translation.insert(VK_BACK, BACK);
		translation.insert(VK_TAB, TAB);
		translation.insert(VK_CLEAR, CLEAR);
		translation.insert(VK_RETURN, RETURN);
		translation.insert(VK_ESCAPE, ESCAPE);

		translation.insert(VK_SELECT, SELECT);
		translation.insert(VK_PRINT, PRINT);

		translation.insert(VK_SNAPSHOT, SNAPSHOT);
		translation.insert(VK_INSERT, INSERT);
		translation.insert(VK_DELETE, DEL); // Windows defines DELETE as a macro!
		translation.insert(VK_PRIOR, PRIOR);
		translation.insert(VK_NEXT, NEXT);
		translation.insert(VK_HOME, HOME);
		translation.insert(VK_END, END);
		translation.insert(VK_HELP, HELP);
		translation.insert(VK_PAUSE, PAUSE);

		translation.insert(VK_APPS, APPS);

		translation.insert(VK_CAPITAL, CAPS_LOCK);
		translation.insert(VK_NUMLOCK, NUM_LOCK);
		translation.insert(VK_SCROLL, SCROLL_LOCK);

		translation.insert(VK_SHIFT, SHIFT);
		translation.insert(VK_LSHIFT, LSHIFT);
		translation.insert(VK_RSHIFT, RSHIFT);
		translation.insert(VK_CONTROL, CONTROL);
		translation.insert(VK_LCONTROL, LCONTROL);
		translation.insert(VK_RCONTROL, RCONTROL);
		translation.insert(VK_MENU, MENU);
		translation.insert(VK_LMENU, LMENU);
		translation.insert(VK_RMENU, RMENU);

		translation.insert(VK_LWIN, LOS);
		translation.insert(VK_RWIN, ROS);

		translation.insert(VK_CANCEL, CANCEL);
		translation.insert(VK_CONVERT, CONVERT);
		translation.insert(VK_NONCONVERT, NONCONVERT);
		translation.insert(VK_ACCEPT, ACCEPT);
		translation.insert(VK_MODECHANGE, MODECHANGE);
		translation.insert(VK_EXECUTE, EXECUTE);

		translation.insert(VK_BROWSER_BACK, BROWSER_BACK);
		translation.insert(VK_BROWSER_FORWARD, BROWSER_FORWARD);
		translation.insert(VK_BROWSER_REFRESH, BROWSER_REFRESH);
		translation.insert(VK_BROWSER_STOP, BROWSER_STOP);
		translation.insert(VK_BROWSER_SEARCH, BROWSER_SEARCH);
		translation.insert(VK_BROWSER_FAVORITES, BROWSER_FAVORITES);
		translation.insert(VK_BROWSER_HOME, BROWSER_HOME);

		translation.insert(VK_VOLUME_MUTE, VOLUME_MUTE);
		translation.insert(VK_VOLUME_DOWN, VOLUME_DOWN);
		translation.insert(VK_VOLUME_UP, VOLUME_UP);

		translation.insert(VK_MEDIA_NEXT_TRACK, MEDIA_NEXT_TRACK);
		translation.insert(VK_MEDIA_PREV_TRACK, MEDIA_PREV_TRACK);
		translation.insert(VK_MEDIA_STOP, MEDIA_STOP);
		translation.insert(VK_MEDIA_PLAY_PAUSE, MEDIA_PLAY_PAUSE);

		translation.insert(VK_LAUNCH_MAIL, LAUNCH_MAIL);
		translation.insert(VK_LAUNCH_MEDIA_SELECT, LAUNCH_MEDIA_SELECT);
		translation.insert(VK_LAUNCH_APP1, LAUNCH_APP1);
		translation.insert(VK_LAUNCH_APP2, LAUNCH_APP2);

		translation.insert(VK_OEM_102, OEM102);
		translation.insert(VK_OEM_CLEAR, OEM_CLEAR);
		translation.insert(VK_PROCESSKEY, PROCESSKEY);
		translation.insert(VK_PACKET, PACKET);

		translation.insert(VK_ATTN, ATTN);
		translation.insert(VK_CRSEL, CRSEL);
		translation.insert(VK_EXSEL, EXSEL);
		translation.insert(VK_EREOF, EREOF);
		translation.insert(VK_PLAY, PLAY);
		translation.insert(VK_ZOOM, ZOOM);
		translation.insert(VK_PA1, PA1);

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
