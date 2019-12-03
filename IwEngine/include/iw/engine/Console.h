#pragma once

#include "Core.h"
#include "iw/events/event.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/linear_allocator.h"
#include <string>
#include<functional>

namespace IW {
inline namespace Engine {
	struct Token {
		union Any {
			float Float;
			double Double;
			int Int;
			long long Long;
			std::string String;
		}* Value;

		size_t Count;
	};

	struct Command {
		std::string* Verb;
		Token* Tokens;
		size_t TokenCount;
	};

	class Console {
	private:
		using handler_func = std::function<bool(const Command&)>;

		std::mutex m_mutex;
		handler_func m_handler;
		iw::linear_allocator m_alloc;
		iw::blocking_queue<std::string> m_commands;

	public:
		IWENGINE_API
		Console(
			const handler_func& handler);

		IWENGINE_API
		void HandleEvent(
			iw::event& e);

		IWENGINE_API
		void HandleCommand(
			const std::string& command);

		IWENGINE_API
		void QueueCommand(
			const std::string& command);

		IWENGINE_API
		void ExecuteQueue();

		// operator <<
	private:
		
	};
}
}
