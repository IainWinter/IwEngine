#include "iw/log/sink/async_sink.h"

namespace iw {
	void async_sink::log(
		std::string& msg)
	{
		m_messages.push(msg);
	}
}
