#pragma once

#include "sink.h"
#include <fstream>

namespace iwlog {
	class IWLOG_API file_sink final
		: public sink
	{
	private:
		struct {
			std::ofstream file;
		};

	public:
		file_sink(
			loglevel level,
			const char* file);

		void log(
			std::string& msg);

		void flush();
	};
}
