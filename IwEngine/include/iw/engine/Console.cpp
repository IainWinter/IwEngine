#include "Console.h"

namespace IW {
	Console::Console(
		const handler_func& handler)
		: m_handler(handler)
		, m_alloc(1024)
	{

	}

	void Console::HandleEvent(
		iw::event& e)
	{
	}

	void Console::HandleCommand(
		const std::string& command)
	{

	}

	void Console::QueueCommand(
		const std::string& command)
	{

	}

	void Console::ExecuteQueue() {
		std::unique_lock<std::mutex> lock(m_mutex);

		while (!m_commands.empty()) {
			HandleCommand(m_commands.pop());
		}

		m_alloc.reset();
	}
}
