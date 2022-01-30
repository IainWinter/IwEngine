#pragma once

#include "iw/log/effects.h"
#include <string>

namespace iw {
namespace log {
	struct sink
	{
		loglevel min_level;

		sink(
			loglevel level
		)
			: min_level (level)
		{}

		virtual ~sink()
		{
			//flush();
		}

		bool should_log(
			loglevel canidate)
		{
			return canidate >= min_level;
		}

		virtual void log(
			loglevel level,
			const std::string& msg) = 0;

		virtual void flush() = 0;
	};
}

	using namespace log;
}
