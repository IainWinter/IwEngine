#pragma once

#include "iw/util/thread/thread_pool.h"
#include "iw/util/memory/ref.h"

#define ASIO_STANDALONE

#include "asio/asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <sstream>

namespace iw
{
namespace util
{
	template<
		typename _t>
	struct NetworkResult
	{
	private:
		ref<_t> m_value;
		ref<bool> m_hasValue;
		ref<std::mutex> m_mutex; // locks has value changed and tests

	public:
		NetworkResult()
			: m_value    (REF<_t>())
			, m_hasValue (REF<bool>(false))
			, m_mutex    (REF<std::mutex>())
		{}

		NetworkResult(
			const NetworkResult& copy
		)
			: m_value    (copy.m_value)
			, m_hasValue (copy.m_hasValue)
			, m_mutex    (copy.m_mutex)
		{}

		NetworkResult(
			NetworkResult&& move
		) noexcept
			: m_value    (std::move(move.m_value))
			, m_hasValue (std::move(move.m_hasValue))
			, m_mutex    (std::move(move.m_mutex))
		{
			move.m_value    = nullptr;
			move.m_hasValue = nullptr;
			move.m_mutex    = nullptr;
		}

		NetworkResult<_t>& operator=(
			const NetworkResult<_t>& copy)
		{
			SetValue(*copy.m_value);
			return *this;
		}

		// not threadsafe for 'move'
		NetworkResult<_t>& operator=(
			NetworkResult<_t>&& move) noexcept
		{
			m_value    = std::move(move.m_value);
			m_hasValue = std::move(move.m_hasValue);
			m_mutex    = std::move(move.m_mutex);

			move.m_value    = nullptr;
			move.m_hasValue = nullptr;
			move.m_mutex    = nullptr;

			return *this;
		}

		void SetValue(
			const _t& value)
		{
			std::unique_lock lock(*m_mutex);
			*m_value = value;
			*m_hasValue = true;
		}

		void SetValue(
			_t&& value)
		{
			std::unique_lock lock(*m_mutex);
			*m_value = std::forward<_t>(value);
			*m_hasValue = true;
		}

		bool HasValue()
		{
			if (!m_mutex)
			{
				return false; // if done with value
			}

			std::unique_lock lock(*m_mutex);
			return *m_hasValue;
		}

		// Once has value is true, this is thread safe so doesnt need to lock?
		_t& Value()
		{
			return *m_value;
		}

		// release data
		void DoneWithValue()
		{
			m_value    = nullptr;
			m_hasValue = nullptr;
			m_mutex    = nullptr;
		}
	};

	// represents a connection to a file on a network through http

	struct NetworkRequest
	{
		std::string Ip;
		std::string Host;
		std::string Resource;
		unsigned short Port;

		virtual std::string GetRequestString() const = 0;
	};

	struct HttpRequest : NetworkRequest
	{
		std::string Ask = "GET";
		std::string Protocall = "HTTP/1.1";
		std::string Connection = "close";

		std::string GetRequestString() const override
		{
			std::stringstream buf;
			buf << Ask
				<< " "  << Resource
				<< " "  << Protocall
				<< "\nHost: " << Host
				<< "\nConnection: " << Connection
				<< "\r\n\r\n";

			return buf.str();

			//std::string request =
			//	"GET /regolith/php/get_highscores.php HTTP/1.1\n"
			//	"Host: data.winter.dev\n"
			//	"Connection: close\r\n\r\n";
		}
	};

	struct NetworkConnection
	{
	private:
		iw::thread_pool m_pool;
		NetworkRequest* m_request;

	public:
		NetworkConnection(
			NetworkRequest* request,
			size_t numberOfThreads = 1
		)
			: m_pool    (numberOfThreads)
			, m_request (request)
		{
			m_pool.init();
		}

		template<
			typename _t,
			typename _s,
			typename _f>
		void Request(
			//const std::string& address,
			_f&& onGetValue)
		{
			using namespace asio;

			asio::error_code e;

			io_context context;
			ip::tcp::endpoint endpoint = ip::tcp::endpoint(ip::make_address(m_request->Ip, e), m_request->Port);

			if (e)
			{
				LOG_ERROR << "Failed to make endpoint " << e.message();
				return;
			}

			// Start thread blocked by idle work

			io_context::work idle(context);
			ip::tcp::socket socket = ip::tcp::socket(context);

			socket.connect(endpoint, e);

			if (e)
			{
				LOG_ERROR << "Failed to connect to socket " << e.message();
				return;
			}

			if (socket.is_open())
			{
				std::string request = m_request->GetRequestString();

				socket.write_some(asio::buffer(request.data(), request.size()));
				socket.wait(socket.wait_read);

				// end once last character is met, I'll put two newlines characters in php script

				std::string buffer; // this could be better

				bool removeHeader = true;
				int attemptsWith0Bytes = 0;

				while (attemptsWith0Bytes < 100)
				{
					size_t bytes = socket.available();

					if (bytes > 0)
					{
						std::string text(bytes, '\0');
						socket.read_some(asio::buffer(text.data(), text.size()));

						if (removeHeader)
						{
							removeHeader = false;
							text = text.substr(text.find("\r\n\r\n") + 4);


							// just use content length for header!! screw the end of transmission thing
						}

						buffer += text; // lot of copying here for no reason, maybe should have header with bytes

						if (text[text.size() - 1] == 4 /* End of Transmission*/)
						{
							break;
						}
					}

					else {
						attemptsWith0Bytes++;
						LOG_WARNING << "Read 0 bytes from network! attempt " << attemptsWith0Bytes << "/100";
					}
				}

				_t record;
				_s(buffer).Read(record);
				onGetValue(record);

				socket.close(e);

				if (e)
				{
					LOG_ERROR << "Error while closing socket: " << e.message();
					return;
				}
			}

			context.stop();
		}

		template<
			typename _t,
			typename _s>
		NetworkResult<_t> AsyncRequest(
			/*const std::string& address*/)
		{
			NetworkResult<_t> result;

			m_pool.queue([&, result]() mutable
			{
				Request<_t, _s>(/*address, */[result](_t& value) mutable
				{
					result.SetValue(value);
				});

				return true;
			});

			return result;
		}
	};

}
	using namespace util;
}
