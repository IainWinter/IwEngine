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

		std::vector<log_time> children;

		void push_time(
			const std::string& name,
			float begin,
			unsigned level)
		{
			if (level > 0) // recurse
			{
				children.back().push_time(name, begin, level - 1);
				return;
			}

			if (children.size() == 0) {
				time = begin;
			}

			children.push_back({ name, begin });
		}

		void pop_time(
			float end,
			unsigned level)
		{
			if (level > 0) // recurse
			{
				children.back().pop_time(end, level - 1);
				return;
			}

			time = end - time;
		}
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
		// only reason for cpp file and this library not being header only is to have
		// the same logger instance for every dll, would like to not need this
		IWLOG_API static std::string log_tags[5];
		IWLOG_API static logger& instance();

		void reset()
		{
			for (sink* sink : m_sinks)
			{
				delete sink;
			}
			m_sinks.clear();

#ifdef IW_LOG_TIME
			clear_times();
#endif
		}

		void flush()
		{
			for (sink* sink : m_sinks)
			{
				sink->flush();
			}
		}

		template<
			typename _sink_t,
			typename... _args_t>
		void make_sink(
			loglevel level,
			_args_t&&... args)
		{
			_sink_t* sink = new _sink_t(level, std::forward<_args_t>(args)...);
			m_sinks.push_back(sink);
			sink->log(level, preamble);
		}

		template<
			typename _t>
		void log(
			loglevel level,
			const _t& obj)
		{
			std::stringstream string; string << obj;
			std::string str = string.str();

			// insert tabs for multi-line messages to match [tags ]

			if (    str.length() > 1
				&& std::count(str.begin(), str.end(), '\n') > 1)
			{
				size_t i = 0;
				while ((i = str.find_first_of('\n', i + 1)) != std::string::npos)
				{
					str.insert(i + 1, "        ", 0, 8);
				}
			}

			sink_msg(level, str);
		}
#ifdef IW_LOG_TIME
		void set_get_time(
			float(*func)())
		{
			m_get_time = func;
		}

		void push_time(
			const std::string& name)
		{
			m_root.push_time(name, m_get_time(), m_current_level);
			m_current_level++;
		}

		void pop_time()
		{
			m_root.pop_time(m_get_time(), m_current_level);
			if (m_current_level > 0) // final pop is at level 0, ends a frame, lil funcky
			{
				m_current_level--;
			}
		}

		log_time&   get_times() { return m_root; }
		void      clear_times() { m_root = {}; }
#endif
#ifdef IW_LOG_VALUE
		void push_value(
			const std::string& name,
			float value)
		{
			m_values[name].push_back(value);
		}

		std::vector<float>&   get_values(const std::string& name) { return m_values[name]; }
		void                clear_values()                        { m_values = {}; }
#endif

	private:
		logger() = default;
		logger(const logger&)  = delete;
		logger(      logger&&) = delete;
		logger& operator=(const logger&)  = delete;
		logger& operator=(      logger&&) = delete;

		void sink_msg(
			loglevel level,
			const std::string& string)
		{
			for (sink* sink : m_sinks)
			{
				if (sink->should_log(level))
				{
					sink->log(level, string);
				}
			}
		}
	};

	struct log_view {
		loglevel m_level;

		log_view(
			loglevel level
		)
			: m_level(level)
		{
			logger::instance().log(m_level, "\n");
			logger::instance().log(m_level, logger::instance().log_tags[(int)level]);
		}

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
	struct log_time_view {
		log_time_view(
			const std::string& name)
		{
			logger::instance().push_time(name);
		}

		~log_time_view()
		{
			logger::instance().pop_time();
		}
	};
#endif
}

	using namespace log;
}

// todo: define when IW_LOG / IW_LOG_TIME / IW_LOG_VALUE are undefed
// all the << ones are kinda broken lol

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
