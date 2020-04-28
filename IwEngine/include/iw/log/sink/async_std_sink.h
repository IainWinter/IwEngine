#pragma once

#include "async_sink.h"

namespace iw {
namespace log {
	class async_stdout_sink
		: public async_sink
	{
	public:
		IWLOG_API
		async_stdout_sink(
			loglevel level);

		IWLOG_API
		void async_log(
			loglevel level,
			std::string& msg) override;

		IWLOG_API
		void flush() override;
	};

	class async_stderr_sink final
		: public async_sink
	{
	public:
		IWLOG_API
		async_stderr_sink(
			loglevel level);

		IWLOG_API
		void async_log(
			loglevel level,
			std::string& msg) override;

		IWLOG_API
		void flush() override;
	};
}

	using namespace log;
}
