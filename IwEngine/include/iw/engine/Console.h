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
			float Number;
			//long long Int; // bits of float vs int make this annoying, just cast to int
			char* String;
		};
	};

	struct Command {
		std::string Original;
		std::string Verb;
		bool Active;
		size_t TokenCount;
		Token* Tokens;

		float Num(int i) const {
			if (i < TokenCount)
				return Tokens[i].Number;
			return 0;
		}

		char* Str(int i) const {
			if (i < TokenCount)
				return Tokens[i].String;
			return 0;
		}
	};

	using HandlerFunc = std::function<bool(const Command&)>;

	class Console {
	private:
		std::mutex m_mutex;
		iw::linear_allocator m_alloc;
		iw::linear_allocator m_strbuf;
		iw::blocking_queue<Command> m_commands;

		std::vector<HandlerFunc*> m_handlers;

	public:
		IWENGINE_API
		Console(
			const HandlerFunc& handler);

		IWENGINE_API
		HandlerFunc* AddHandler(
			HandlerFunc handler);

		IWENGINE_API
		void RemoveHandler(
			HandlerFunc* handler);

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

		void SendCommandEvents(
			Command& c);
	};
}

	using namespace Engine;
}
