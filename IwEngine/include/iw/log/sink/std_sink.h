#pragma once

#include "sink.h"
#include <iostream>

namespace iw {
namespace log {
	struct stdout_sink
		: sink
	{
		stdout_sink(
			loglevel level
		)
			: sink (level)
		{}

		virtual ~stdout_sink() {}

		void log(
			loglevel level,
			const std::string& msg) override
		{
			set_color(level);
			std::cout << msg;
		}

		void flush() override
		{
			std::cout.flush();
		}
	};

	struct stderr_sink 
		: sink
	{
		stderr_sink(
			loglevel level
		)
			: sink(level)
		{}

		virtual ~stderr_sink() {}

		void log(
			loglevel level,
			const std::string& msg) override
		{
			set_color(level);
			std::cerr << msg;
		}

		void flush() override
		{
			std::cerr.flush();
		}
	};
}

	using namespace log;
}
