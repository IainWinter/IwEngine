#pragma once

#include "iwlog.h"
#include "sink/sink.h"
#include <vector>
#include <sstream>
#include <string>

#define IW_LOG_TIME

namespace iw {
namespace log {
#ifdef IW_LOG_TIME
	struct log_time {
		std::string name = "Frame";
		float time = 0;

		unsigned my_level = 0;
		unsigned my_index = 0;
		std::vector<log_time> children;

		void push_time(
			std::string& name,
			float begin,
			unsigned level);

		void pop_time(
			float end,
			unsigned current_level);
	};
#endif
	class logger {
	private:
		std::vector<sink*> m_sinks;
		std::string preamble = "[Pream] Start of log...";
#ifdef IW_LOG_TIME
		float(*m_get_time)();
		log_time m_root = {};
		unsigned m_current_level = 0;
#endif
	public:
		template<
			typename _sink_t,
			typename... _Args_T>
		void make_sink(
			loglevel level,
			_Args_T&& ... args)
		{
			_sink_t* sink = new _sink_t(level, std::forward<_Args_T>(args)...);
			m_sinks.push_back(sink);

			sink->log(INFO, preamble);
		}

		template<
			typename _t>
		void log(
			loglevel level,
			const _t& obj)
		{
			std::stringstream string;
			string << obj;

			std::string str = string.str();
			if (    str.length() > 1
				&& std::count(str.begin(), str.end(), '\n') > 1)
			{
				size_t i = 0;
				while ((i = str.find_first_of('\n', i + 1)) != std::string::npos) {
					str.insert(i + 1, "        ", 0, 8);
				}
			}

			sink_msg(level, str);
		}
#ifdef IW_LOG_TIME
		IWLOG_API
		void set_get_time(
			float(*func)());

		IWLOG_API
		void push_time(
			std::string name);

		IWLOG_API
		void pop_time();

		IWLOG_API
		log_time get_times();

		IWLOG_API
		void clear_times();
#endif
		IWLOG_API
		void flush();

		IWLOG_API
		static logger& instance();
	private:
		logger() = default;
		logger(logger const&) = delete;
		logger(logger&&) = delete;

		IWLOG_API
		void sink_msg(
			loglevel level,
			std::string& string);
	};

	class log_view {
	private:
		loglevel m_level;

	public:
		IWLOG_API
		log_view(
			loglevel level);

		template<
			typename _t>
		log_view& operator<<(
			const _t& msg)
		{
			logger::instance().log(m_level, msg);
			return *this;
		}
	};
#ifdef IW_LOG_TIME
	class log_time_view {
	public:
		IWLOG_API
		log_time_view(
			std::string name);

		IWLOG_API
		~log_time_view();
	};
#endif
}

	using namespace log;
}

#define LOG_SINK(_type_, _level_, ...)                              \
	iw::logger::instance().make_sink<_type_>(_level_, __VA_ARGS__)

#define LOG_FLUSH   iw::logger::instance().flush

#define LOG_INFO     iw::log_view(iw::INFO)
#define LOG_DEBUG    iw::log_view(iw::DEBUG)
#define LOG_WARNING  iw::log_view(iw::WARN)
#define LOG_ERROR    iw::log_view(iw::ERR)
#define LOG_TRACE    iw::log_view(iw::TRACE)

#ifdef IW_LOG_TIME
#	define LOG_SET_GET_TIME(func) logger::instance().set_get_time(func);
#	define LOG_TIME_SCOPE(time) iw::log_time_view __keep_alive = iw::log_time_view(time)
#	define LOG_GET_TIMES() logger::instance().get_times()
#	define LOG_CLEAR_TIMES() logger::instance().clear_times()
#endif
