#pragma once

#include "sink/sink.h"
#include <vector>
#include <sstream>

#define IW_LOG_TIME
#define IW_LOG_VALUE

#ifdef IW_LOG_VALUE
#	include <unordered_map>
#endif

namespace iw {
namespace log {
#ifdef IW_LOG_TIME
	struct log_time {
		std::string name = "Frame";
		float time = 0;

		//unsigned my_level = 0;
		//unsigned my_index = 0;
		std::vector<log_time> children;

		void push_time(
			const std::string& name,
			float begin,
			unsigned level);

		void pop_time(
			float end,
			unsigned level);
	};
#endif
	class logger {
	private:
		std::vector<sink*> m_sinks;
		std::string preamble = "[Pream] Start of log...";
#ifdef IW_LOG_TIME
		float(*m_get_time)()     = nullptr;
		log_time m_root          = {};
		unsigned m_current_level = 0;
#endif
#ifdef IW_LOG_VALUE
		std::unordered_map<std::string, std::vector<float>> m_values;
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

			sink->log(level, preamble);
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
			const std::string& name);

		IWLOG_API
		void pop_time();

		IWLOG_API
		log_time get_times();

		IWLOG_API
		void clear_times();
#endif

#ifdef IW_LOG_TIME
		IWLOG_API
		void push_value(
			const std::string& name,
			float value);

		IWLOG_API
		const std::vector<float>& get_values(
			const std::string& name);

		IWLOG_API
		void clear_values();
#endif
		IWLOG_API
		void reset();

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
	class IWLOG_API log_time_view {
	public:
		log_time_view(
			const std::string& name);

		~log_time_view();
	};
#endif
}

	using namespace log;
}

// todo: define when not IW_LOG / IW_LOG_TIME / IW_LOG_VALUE are undefed

#define LOG_RESET iw::logger::instance().reset
#define LOG_FLUSH iw::logger::instance().flush

#define LOG_SINK(_type_, _level_, ...)                              \
	iw::logger::instance().make_sink<_type_>(_level_, __VA_ARGS__)

#define LOG_INFO    iw::log_view(iw::loglevel::INFO)
#define LOG_DEBUG   iw::log_view(iw::loglevel::DEBUG)
#define LOG_WARNING iw::log_view(iw::loglevel::WARN)
#define LOG_ERROR   iw::log_view(iw::loglevel::ERR)
#define LOG_TRACE   iw::log_view(iw::loglevel::TRACE)

#ifdef IW_LOG_TIME
#	define LOG_GET_TIMES()        iw::logger::instance().get_times()
#	define LOG_CLEAR_TIMES()      iw::logger::instance().clear_times()
#	define LOG_FINISH_TIMES()     iw::logger::instance().pop_time()
#	define LOG_SET_GET_TIME(func) iw::logger::instance().set_get_time(func);
#	define LOG_TIME_SCOPE(name)   iw::log_time_view __keep_alive = iw::log_time_view(name)
#endif

#ifdef IW_LOG_VALUE
#	define LOG_VALUE(name, value) iw::logger::instance().push_value(name, value)
#endif
