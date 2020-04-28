#include "iw/log/sink/std_sink.h"
#include <iostream>

namespace iw {
	stdout_sink::stdout_sink(
		loglevel level)
		: sink(level)
	{}

	void stdout_sink::log(
		loglevel level,
		std::string& msg)
	{
		set_color(level);
		std::cout << msg;
	}

	void stdout_sink::flush() {
		std::cout.flush();
	}

	stderr_sink::stderr_sink(
		loglevel level)
		: sink(level)
	{}

	void stderr_sink::log(
		loglevel level,
		std::string& msg)
	{
		set_color(level);
		std::cerr << msg;
	}

	void stderr_sink::flush() {
		std::cerr.flush();
	}
}
