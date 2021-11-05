#pragma once

#include "sink.h"
#include "iw/log/iwlog.h"
#include "iw/util/queue/blocking_queue.h"
#include <string>

namespace iw {
namespace log {
	class async_sink
		: public sink
	{
	private:
		struct logbit {
			loglevel level;
			std::string text;

			bool poison = false;
		};

		iw::blocking_queue<logbit> m_messages;
		std::thread m_thread;

	private:
		IWLOG_API
		void async_logger() {
			while (true) {
				logbit log = m_messages.pop();
				if (log.poison) break;

				async_log(log.level, log.text);
			}
		}
	public:
		IWLOG_API
		async_sink(
			loglevel level)
			: sink(level)
			, m_thread([this] { async_logger(); })
		{}

		IWLOG_API
		virtual ~async_sink() {
			m_messages.push({loglevel::INFO, "", true});
			m_thread.join(); // why does this error?
		}

		IWLOG_API
		void log(
			loglevel level,
			std::string& msg);

		IWLOG_API
		virtual void async_log(
			loglevel level,
			std::string& msg) = 0;
	};
}

	using namespace log;
}
