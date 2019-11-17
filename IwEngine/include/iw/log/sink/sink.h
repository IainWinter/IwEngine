#pragma once

#include "iw/log/iwlog.h"
#include <string>

namespace iw {
inline namespace log {
	class IWLOG_API sink {
	protected:
		loglevel min_level;

	public:
		sink(loglevel level)
			: min_level(level) {}

		virtual ~sink() {}

		virtual void log(
			std::string& msg) = 0;

		virtual void flush() = 0;

		inline bool should_log(
			loglevel canidate)
		{
			return canidate >= min_level;
		}
	};
}
}
