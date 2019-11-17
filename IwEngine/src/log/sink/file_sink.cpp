#include "iw/log/sink/file_sink.h"

namespace iw {
	file_sink::file_sink(
		loglevel level,
		const char* file)
		: sink(level)
		, file(std::ofstream(file))
	{}

	void file_sink::log(
		std::string& msg)
	{
		file << msg;
	}

	void file_sink::flush() {
		file.flush();
	}
}
