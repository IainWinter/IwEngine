#pragma once

#include "iw/util/thread/thread_pool.h"
#include "iw/util/memory/ref.h"

#define ASIO_STANDALONE

#include "asio/asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <sstream>
#include <unordered_map>

namespace iw
{
namespace util
{
	template<
		typename _t>
	struct NetworkResult
	{
		using _value_t = _t;

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

	// placeholder for no serializtion
	// causes NetworkConnection::Result to return string
	struct NoSerializer
	{};

	template<
		typename _t,
		typename _s = NoSerializer>
	struct NetworkRequest
	{
		using _serializer_t = _s;

		using _result_t = std::conditional_t<
			std::is_same_v<_s, NoSerializer>,
				std::string,
				_t>;

		using _onResult_t = std::function<void(_result_t&)>;

		std::string Ip;
		std::string Host;
		std::string Resource;
		unsigned short Port;

		_serializer_t* Serializer;
		_onResult_t OnResult;

		NetworkRequest()
			: Port       (0)
			, Serializer (nullptr)
		{}

		virtual std::string GetRequestString() const = 0;

		// lil weird to have this here but
		// returns if done receiving
		// when done, buffer should be ready for deserialization
		virtual bool ProcessBuffer(std::string& buffer)= 0;

		static _result_t Respond(
			const std::string& str)
		{
			if constexpr (std::is_same_v<_s, NoSerializer>)
			{
				return str;
			}

			else
			{
				_t record;
				_serializer_t(str).Read(record);
				return record;
			}
		}
	};

	template<
		typename _t,
		typename _s = NoSerializer>
	struct HttpRequest
		: NetworkRequest<_t, _s>
	{
		std::string Ask = "GET";
		std::string Protocall = "HTTP/1.1";
		std::string Connection = "close";
		std::unordered_map<std::string, std::string> Arguments;

	private:
		bool foundContentLength = false; // little weird to have state here
		bool foundHeader = false;        // but this struct gets copied in the Request function so it doesnt matter
		size_t contentLength = 0; 

	public:
		HttpRequest()
			: NetworkRequest<_t, _s>()
		{
			Port = 80;
		}

		std::string GetRequestString() const override
		{
			std::stringstream buf;
			buf << Ask
			    << " " << Resource;

			bool first = true;
			for (auto& [name, arg] : Arguments)
			{
				buf << (first ? "?" : "&")
				    << name << "=" << arg;
				first = false;
			}

			buf << " "              << Protocall
			    << "\nHost: "       << Host
			    << "\nConnection: " << Connection
			    << "\r\n\r\n";

			return buf.str();
		}

		bool ProcessBuffer(
			std::string& buffer) override
		{
			if (!foundContentLength)
			{
				const char* clName = "Content-Length:";
				size_t clIndex = buffer.find(clName);

				if (clIndex != size_t(-1))
				{
					clIndex += strlen(clName) + 1;
					size_t clIndexEnd = buffer.find("\r\n", clIndex);

					foundContentLength = clIndexEnd != size_t(-1);

					if (foundContentLength)
					{
						contentLength = std::stoull(buffer.substr(clIndex, clIndexEnd - clIndex));
					}
				}
			}

			if (!foundHeader)
			{
				size_t endIndex = buffer.find("\r\n\r\n");

				foundHeader = endIndex != size_t(-1);

				if (foundHeader)
				{
					buffer = buffer.substr(endIndex + 4);
					buffer.reserve(contentLength);
				}
			}

			return buffer.size() == contentLength;
		}

		template<
			typename _t>
		void SetArgument(
			const std::string& name,
			const _t& value)
		{
			std::stringstream ss;
			ss << value;
			Arguments[name] = ss.str();
		}

		void RemoveArgument(
			const std::string& name)
		{
			Arguments.erase(name);
		}
	};

	struct NetworkConnection
	{
	private:
		iw::thread_pool m_pool;

	public:
		NetworkConnection(
			size_t numberOfThreads = 1
		)
			: m_pool (numberOfThreads)
		{
			m_pool.init();
		}

		void StepResults()
		{
			m_pool.step_coroutines();
		}

		// makes a copy of network request because it has internal state
		template<
			typename _r>
		typename _r::_result_t Request(
			_r networkRequest)
		{
			std::string request = networkRequest.GetRequestString();

			if (request.size() == 0)
			{
				LOG_WARNING << "Invalude request based on fields!";
				return {};
			}

			using namespace asio;

			asio::error_code e;

			io_context context;
			ip::tcp::endpoint endpoint = ip::tcp::endpoint(
				ip::make_address(networkRequest.Ip, e),
				networkRequest.Port);

			if (e)
			{
				LOG_ERROR << "Failed to make endpoint " << e.message();
				return {};
			}

			// Start thread blocked by idle work

			io_context::work idle(context);
			ip::tcp::socket socket = ip::tcp::socket(context);

			socket.connect(endpoint, e);

			if (e)
			{
				LOG_ERROR << "Failed to connect to socket " << e.message();
				return {};
			}

			if (socket.is_open())
			{
				socket.write_some(asio::buffer(request.data(), request.size()));
				
				// end once last character is met, I'll put two newlines characters in php script

				std::string buffer; // this could be better

				while (true)
				{
					socket.wait(socket.wait_read);
					size_t bytes = socket.available();

					std::string text(bytes, '\0');
					socket.read_some(asio::buffer(text.data(), text.size()));
					buffer += text; // lot of copying here for no reason, maybe should have header with bytes

					bool done = networkRequest.ProcessBuffer(buffer);

					if (done)
					{
						break;
					}
				}

				socket.close(e);

				if (e)
				{
					LOG_ERROR << "Error while closing socket: " << e.message();
					return {};
				}

				return _r::Respond(buffer);
			}

			context.stop();

			return {};
		}

		// request is copied and is free to edit immediately after call
		template<
			typename _r>
		NetworkResult<typename _r::_result_t> AsyncRequest(
			const _r& netRequest)
		{
			NetworkResult<_r::_result_t> result;

			// run the request on a seperate thread
			m_pool.queue(
				[this, netRequest, result]() mutable
				{
					result.SetValue(Request<_r>(netRequest));
					return true;
				}
			);

			if (netRequest.OnResult)
			{
				_r::_onResult_t onResult = netRequest.OnResult;

				// poll for the result on the main thread
				m_pool.coroutine(
					[result, onResult]() mutable
					{
						if (result.HasValue())
						{
							onResult(result.Value());
							result.DoneWithValue();
							return true;
						}

						return false;
					}
				);
			}

			return result;
		}
	};

}
	using namespace util;
}
