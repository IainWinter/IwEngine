#include "iw/log/effects.h"

#ifdef IW_PLATFORM_WINDOWS
#	include <Windows.h>
#endif


namespace iw {
namespace log {
	void set_color(
		loglevel level)
	{
#ifdef IW_PLATFORM_WINDOWS
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD effect = 15;

		switch (level) {
			case loglevel::INFO:  effect = 7;  break;
			case loglevel::DEBUG: effect = 15; break;
			case loglevel::WARN:  effect = 14; break;
			case loglevel::ERR:   effect = 12; break;
			case loglevel::TRACE: effect = 20; break;
		}

		SetConsoleTextAttribute(hConsole, effect);
#endif
	}
}
}
