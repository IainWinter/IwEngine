#pragma once

#include "iw/log/effects.h"
#include <string>

namespace iw {
namespace log {
	class sink {
	protected:
		loglevel min_level;

	public:
		sink(
			loglevel level)
			: min_level(level) {}

		IWLOG_API
		virtual ~sink() = default;

		IWLOG_API
		virtual void log(
			loglevel level,
			std::string& msg) = 0;

		IWLOG_API
		virtual void flush() = 0;

		inline bool should_log(
			loglevel canidate)
		{
			return canidate >= min_level;
		}
	};
}

	using namespace log;
}
