#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWLOG_API __declspec(dllexport)
#else
#		define IWLOG_API __declspec(dllimport)
#	endif
#endif

#define iwlog iwl

namespace iwlog {
	enum loglevel {
		INFO,
		DEBUG,
		WARN,
		ERR,
		TRACE
	};
}
