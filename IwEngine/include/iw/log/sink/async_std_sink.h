#pragma once

#include "async_sink.h"
#include <iostream>

namespace iw {
namespace log {
	struct async_stdout_sink
		: async_sink
	{
	public:
		async_stdout_sink(
			loglevel level
		)
			: async_sink (level)
		{}

		virtual ~async_stdout_sink() {}

		void async_log(
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

	struct async_stderr_sink
		: async_sink
	{
	public:
		async_stderr_sink(
			loglevel level
		)
			: async_sink (level)
		{}

		virtual ~async_stderr_sink() {}

		void async_log(
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
