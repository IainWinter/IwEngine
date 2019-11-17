#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWLOG_API __declspec(dllexport)
#else
#		define IWLOG_API __declspec(dllimport)
#	endif
#endif

namespace iw {
inline namespace log {
	enum loglevel {
		INFO,
		DEBUG,
		WARN,
		ERR,
		TRACE
	};
}
}
