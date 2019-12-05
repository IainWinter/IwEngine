#include "iw/engine/Console.h"
#include <sstream>

namespace IW {
	Console::Console(
		const HandlerFunc& handler)
		: m_handler(handler)
		, m_alloc(1024)
		, m_strbuf(1024)
	{

	}

	void Console::HandleEvent(
		iw::event& e)
	{

	}

	void Console::ExecuteCommand(
		const std::string& command)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		Command c = AllocCommand(command);
		m_handler(c);
	}

	void Console::QueueCommand(
		const std::string& command)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		Command c = AllocCommand(command);
		m_commands.push(c);
	}

	void Console::ExecuteQueue() {
		std::unique_lock<std::mutex> lock(m_mutex);

		while (!m_commands.empty()) {
			m_handler(m_commands.pop());
		}

		m_alloc.reset();
		m_strbuf.reset();
	}

	Command Console::AllocCommand(
		const std::string& command)
	{
		std::vector<std::string> tokens;

		std::string token;
		std::stringstream stream(command);
		while (std::getline(stream, token, ' ')) {
			tokens.push_back(token);
		}

		char* original = m_strbuf.alloc<char>(tokens[0].size() + 1);
		strcpy(original, tokens[0].c_str());
			 
		int active = -1;
		char* verb = nullptr;
		if (tokens[0][0] == '+' || tokens[0][0] == '-') {
			active = tokens[0][0] == '+';
			verb = m_strbuf.alloc<char>(tokens[0].size() + 0);
			strcpy(verb, tokens[0].substr(1).c_str());
		}

		else {
			verb = m_strbuf.alloc<char>(tokens[0].size() + 1);
			strcpy(verb, tokens[0].c_str());
		}

		Token* ts = nullptr;

		size_t tsize  = sizeof(Token);
		size_t tcount = tokens.size();
		for (size_t i = 1; i < tcount; i++) {
			Token* t = (Token*)m_alloc.alloc(tsize);
			bool error = false;

			size_t itr;
			size_t tend = tokens[i].size();

			long long ll;
			try {
				ll = std::stol(tokens[i], &itr);
			}

			catch (std::invalid_argument e) { error = true; }
			catch (std::out_of_range e) { error = true; }

			if (!error && itr == tend) {
				t->Int = ll;
				t->Count = 1;
			}

			else {
				float f;
				try {
					f = std::stof(tokens[i], &itr);
					error = false;
				}

				catch (std::invalid_argument e) { error = true; }
				catch (std::out_of_range e) { error = true; }

				if (!error && itr == tend) {
					t->Float = f;
					t->Count = 1;
				}

				else {
					char* str = m_strbuf.alloc<char>(tend + 1);
					strcpy(str, tokens[i].c_str());

					t->Count = tokens[i].size();
					t->String = str;
				}
			}

			if (ts == nullptr) {
				ts = t;
			}
		}

		Command c {
			std::string(original),
			std::string(verb),
			active,
			tcount - 1,
			ts
		};

		return c;
	}

	Token Console::ParseToken(
		const std::string& command)
	{
		return Token();
	}
}
