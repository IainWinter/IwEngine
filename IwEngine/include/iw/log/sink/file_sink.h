#pragma once

#include "sink.h"
#include <fstream>

namespace iw {
namespace log {
	class file_sink
		: public sink
	{
	private:
		std::ofstream file;

	public:
		IWLOG_API
		file_sink(
			loglevel level,
			const char* file);

		IWLOG_API
		void log(
			loglevel level,
			std::string& msg);

		IWLOG_API
		void flush();
	};
}

	using namespace log;
}
