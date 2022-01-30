#pragma once

#include "iw/log/iwlog.h"
#include "iw/common/Platform.h"
// could just include windows,
// this is just to keep things clean,
// but now this depeneds on common :<

namespace iw {
namespace log {
	inline void set_color(
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

	using namespace log;
}
