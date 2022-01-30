#pragma once

#include "sink.h"
#include <fstream>

namespace iw {
namespace log {
	struct file_sink
		: sink
	{
		std::ofstream file;

		file_sink(
			loglevel level,
			const char* file
		)
			: sink (level)
			, file (std::ofstream(file))
		{}

		virtual ~file_sink()
		{
			file.close();
		}

		void log(
			loglevel level,
			const std::string& msg) override
		{
			file << msg;
		}

		void flush() override
		{
			file.flush();
		}
	};
}

	using namespace log;
}
