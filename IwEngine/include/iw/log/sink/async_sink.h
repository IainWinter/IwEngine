#pragma once

#include "sink.h"
#include "iw/log/iwlog.h"
#include "iw/util/queue/blocking_queue.h"
#include <string>

namespace iwlog {
	class IWLOG_API async_sink
		: public sink
	{
	private:
		iwu::blocking_queue<std::string> m_messages;
		std::thread                      m_thread;

	public:
		async_sink(
			loglevel level)
			: sink(level)
			, m_thread([this]() { while (true) { async_log(m_messages.pop()); } })
		{}

		virtual ~async_sink() {
			m_thread.join();
		}

		void log(
			std::string& msg);

		virtual void async_log(
			std::string& msg) = 0;
	};
}
