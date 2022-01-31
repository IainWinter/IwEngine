#pragma once

#include "sink.h"
#include "iw/util/queue/blocking_queue.h"
#include <thread>

namespace iw {
namespace log {
	struct async_sink
		: sink
	{
		struct logbit {
			loglevel level;
			std::string text;

			bool poison = false;
		};

		iw::blocking_queue<logbit> m_messages;
		std::thread m_thread;

		async_sink(
			loglevel level
		)
			: sink     (level)
			, m_thread ([this] { async_logger(); })
		{}

		virtual ~async_sink() // this stil crashes :( really not sure why
		{
			m_messages.push({loglevel::INFO, "", true});
			m_thread.join();
		}

		void log(
			loglevel level,
			const std::string& msg) override
		{
			m_messages.push({ level, msg });
		}

		void async_logger()
		{
			while (true)
			{
				logbit log = m_messages.pop();
				if (log.poison)
				{
					break;
				}

				async_log(log.level, log.text);
			}
		}

		virtual void async_log(
			loglevel level,
			const std::string& msg) = 0;
	};
}

	using namespace log;
}
