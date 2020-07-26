#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWLOG_DLL
#		define IWLOG_API __declspec(dllexport)
#else
#		define IWLOG_API __declspec(dllimport)
#	endif
#endif

namespace iw {
namespace log {
	enum loglevel {
		INFO,
		DEBUG,
		WARN,
		ERR,
		TRACE,
		TIME,
	};
}

	using namespace log;
}
