#pragma once

#include <string>
#include "./util/tsque.h"

#define ASIO_STANDALONE
#include "asio/asio.hpp"
//#include <Windows.h>

// almost directly copies from olc
// just used the core

template<typename _id>
struct message_header
{
	_id m_id;
	uint32_t m_size; // should technically be 64bit, but then id gets padded
};

template<typename _id>
struct message
{
	message_header<_id> m_header;
	std::string m_text;
};

template<typename _id>
message<_id> make_message(_id id, const std::string& text)
{
	return {
		{ id, (uint32_t)text.size() },
		text
	};
}

template<typename _id>
struct session;

template<typename _id>
struct owned_message : message<_id>
{
	std::shared_ptr<session<_id>> m_session;
};

template<typename _id>
struct session : std::enable_shared_from_this<session<_id>>
{
    asio::ip::tcp::socket m_socket;
	tsque<owned_message<_id>>& m_in;
	tsque<message<_id>> m_out;

	owned_message<_id> m_current; // incoming built here

    session(
		asio::ip::tcp::socket&& socket,
		tsque<owned_message<_id>>& incomming
	)
    	: m_socket (std::move(socket))
		, m_in     (incomming)
    {}

	void start_listening()
	{
		m_current.m_session = this->shared_from_this();
		read_header();
	}

	void write(const message<_id>& msg)
	{
		bool ready = m_out.empty();
		m_out.push_back(msg);
		if (ready) write_header();
	}

private:
    void read_header()
    {
		asio::async_read(m_socket, asio::buffer(&m_current.m_header, sizeof(message_header<_id>)), 
			[this](
				std::error_code error, std::size_t size)
			{
				printf("received %d bytes\n", (int)size);

				if (error)
				{
					printf("Error reading incomming message header: %s\n", error.message().c_str());
					m_socket.close();
				}

				else if (m_current.m_header.m_size == 0) // this is an empty message so add it but dont read body
				{
					m_in.push_back(m_current);
				}

				else
				{
					m_current.m_text.resize(m_current.m_header.m_size);
					read_body();
				}
			});
	}

	void read_body()
	{
		asio::async_read(m_socket, asio::buffer(m_current.m_text.data(), m_current.m_text.size()), 
			[this](
				std::error_code error, std::size_t size)
			{
				printf("received %d bytes\n", (int)size);

				if (error)
				{
					printf("Error reading incomming message body: %s\n", error.message().c_str());
					m_socket.close();
				}

				else
				{
					m_in.push_back(m_current);
					read_header(); // wait for another header
				}
			});
    }

	void write_header()
	{
		asio::async_write(m_socket, asio::buffer(&m_out.front().m_header, sizeof(message_header<_id>)),
			[this](
				std::error_code error,
				size_t size)
			{
				printf("sent %d bytes\n", (int)size);

				if (error)
				{
					printf("Error writing outgoing message header: %s\n", error.message().c_str());
					m_socket.close();
				}

				else
				{
					if (m_out.front().m_text.size() > 0)
					{
						write_body();
					}

					else
					{
						m_out.pop_front();

						if (!m_out.empty())
						{
							write_header(); // write the next message if one exists
						}
					}
				}
			});
	}

	void write_body()
	{
		asio::async_write(m_socket, asio::buffer(m_out.front().m_text, m_out.front().m_text.size()),
			[this](
				std::error_code error,
				size_t size)
			{
				printf("sent %d bytes\n", (int)size);
				
				if (error)
				{
					printf("Error writing outgoing message body: %s\n", error.message().c_str());
					m_socket.close();
				}

				else
				{
					m_out.pop_front();

					if (!m_out.empty())
					{
						write_header(); // write the next message if one exists
					}
				}
			});		
	}
};

template<typename _id>
struct server
{
    asio::io_context& io_context;
    asio::ip::tcp::acceptor acceptor;

	tsque<owned_message<_id>> m_in;
	tsque<std::shared_ptr<session<_id>>> m_sessions;

    server(
		asio::io_context& io_context, 
		uint16_t port
	)
    	: io_context (io_context)
    	, acceptor   (io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {}

    void async_accept()
    {
        acceptor.async_accept(
			[this](
				std::error_code error, 
				asio::ip::tcp::socket socket)
        	{
				if (error)
				{
					printf("Error with incomming connection: %s\n", error.message().c_str());
				}

				else
				{
					for (int i = 0; i < m_sessions.size(); i++)
					{
						if (m_sessions.at(i).use_count() == 2) // m_sessions owns this and m_current inside of the session owns it
						{
							m_sessions.erase(i); i--;
						}
					}

					std::shared_ptr<session<_id>> conn = std::make_shared<session<_id>>(std::move(socket), m_in);
					m_sessions.push_back(conn);
					
					conn->start_listening();

					printf("Client connected to server, number of sessions:  %d\n", (int)m_sessions.size());
				}

            	async_accept();
        	});
    }
};
