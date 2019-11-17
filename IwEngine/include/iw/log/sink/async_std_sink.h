#pragma once

#include "async_sink.h"

namespace iw {
inline namespace log {
	class IWLOG_API async_stdout_sink final
		: public async_sink
	{
	public:
		async_stdout_sink(
			loglevel level);

		void async_log(
			std::string& msg) override;

		void flush() override;
	};

	class IWLOG_API async_stderr_sink final
		: public async_sink
	{
	public:
		async_stderr_sink(
			loglevel level);

		void async_log(
			std::string& msg) override;

		void flush() override;
	};
}
}
