#include "iw/log/sink/async_sink.h"

namespace iw {
	void async_sink::log(
		loglevel level,
		std::string& msg)
	{
		m_messages.push({ level, msg });
	}
}
