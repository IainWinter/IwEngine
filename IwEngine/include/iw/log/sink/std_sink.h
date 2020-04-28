#pragma once

#include "sink.h"

namespace iw {
namespace log {
	class IWLOG_API stdout_sink final
		: public sink
	{
	public:
		stdout_sink(
			loglevel level);

		void log(
			loglevel level,
			std::string& msg);

		void flush();
	};

	class IWLOG_API stderr_sink final
		: public sink
	{
	public:
		stderr_sink(
			loglevel level);

		void log(
			loglevel level,
			std::string& msg);

		void flush();
	};
}

	using namespace log;
}
