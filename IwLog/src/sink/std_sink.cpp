#include "sink/std_sink.h"
#include <iostream>

namespace iwlog {
	stdout_sink::stdout_sink(
		loglevel level)
		: sink(level)
	{}

	void stdout_sink::log(
		std::string& msg)
	{
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
		std::string& msg)
	{
		std::cerr << msg;
	}

	void stderr_sink::flush() {
		std::cerr.flush();
	}
}
