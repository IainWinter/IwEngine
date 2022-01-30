#include "iw/log/logger.h"

namespace iw {
namespace log {
	logger& logger::instance()
	{
		static logger log;
		return log;
	}

	std::string logger::log_tags[5] = {
		"[Info ] ",
		"[Debug] ",
		"[Warn ] ",
		"[Error] ",
		"[Trace] "
	};
}
}
