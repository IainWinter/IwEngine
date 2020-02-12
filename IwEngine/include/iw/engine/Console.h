#pragma once

#include "Core.h"
#include "iw/events/event.h"
#include "iw/events/callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/linear_allocator.h"
#include <string>
#include <functional>

namespace iw {
namespace Engine {
	struct Token {
		size_t Count;

		union {
			float Float;
			long long Int;
			char* String;
		};
	};

	struct Command {
		std::string Original;
		std::string Verb;
		bool Active;
		size_t TokenCount;
		Token* Tokens; // size is token count
	};

	class Console {
	public:
		using HandlerFunc = iw::getback<bool, const Command&>;

	private:
		std::mutex m_mutex;
		iw::linear_allocator m_alloc;
		iw::linear_allocator m_strbuf;
		iw::blocking_queue<Command> m_commands;

		HandlerFunc m_handler;

	public:
		IWENGINE_API
		Console(
			const HandlerFunc& handler);

		IWENGINE_API
		void HandleEvent(
			iw::event& e);

		IWENGINE_API
		void ExecuteCommand(
			const std::string& command);

		IWENGINE_API
		void QueueCommand(
			const std::string& command);

		IWENGINE_API
		void ExecuteQueue();
	private:
		Command AllocCommand(
			const std::string& command);

		Token ParseToken(
			const std::string& command);
	};
}

	using namespace Engine;
}
