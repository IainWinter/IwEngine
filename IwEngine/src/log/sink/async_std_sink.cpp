#include "iw/log/sink/async_std_sink.h"
#include <iostream>

namespace iw {
	async_stdout_sink::async_stdout_sink(
		loglevel level)
		: async_sink(level)
	{}

	void async_stdout_sink::async_log(
		loglevel level,
		std::string& msg)
	{
		set_color(level);
		std::cout << msg;
	}

	void async_stdout_sink::flush() {
		std::cout.flush();
	}

	async_stderr_sink::async_stderr_sink(
		loglevel level)
		: async_sink(level)
	{}

	void async_stderr_sink::async_log(
		loglevel level,
		std::string& msg)
	{
		set_color(level);
		std::cerr << msg;
	}

	void async_stderr_sink::flush() {
		std::cerr.flush();
	}
}
