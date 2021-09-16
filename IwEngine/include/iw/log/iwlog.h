#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWLOG_DLL
#		define IWLOG_API __declspec(dllexport)
#	else
#		define IWLOG_API __declspec(dllimport)
#	endif
#else
#	define IWLOG_API
#endif

namespace iw {
namespace log {
	enum class loglevel
		: int
	{
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
