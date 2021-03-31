#include "iw/log/logger.h"

namespace iw {
	static std::string tags[5] = {
		"[Info ] ",
		"[Debug] ",
		"[Warn ] ",
		"[Error] ",
		"[Trace] "
	};

	void logger::sink_msg(
		loglevel level,
		std::string& string)
	{
		for (sink* sink : m_sinks) {
			if (sink->should_log(level)) {
				sink->log(level, string);
			}
		}
	}
#ifdef IW_LOG_TIME
	void logger::set_get_time(
		float(*func)())
	{
		m_get_time = func;
	}

	void logger::push_time(
		std::string name)
	{
		m_root.push_time(name, m_get_time(), m_current_level);
		m_current_level++;
	}

	void logger::pop_time() {
		m_root.pop_time(m_get_time(), m_current_level);
		m_current_level--;
	}

	log_time logger::get_times() {
		return m_root;
	}

	void logger::clear_times() {
		m_root = {};
	}
#endif
	void logger::reset() {
		for (sink* sink : m_sinks) {
			delete sink;
		}
		m_sinks.clear();

#ifdef IW_LOG_TIME
		clear_times();
#endif
	}

	void logger::flush() {
		for (sink* sink : m_sinks) {
			sink->flush();
		}
	}

	logger& logger::instance() {
		static logger log;
		return log;
	}

	log_view::log_view(
		loglevel level)
		: m_level(level)
	{
		logger::instance().log(m_level, "\n");
		logger::instance().log(m_level, tags[(int)level]);
	}
#ifdef IW_LOG_TIME
	log_time_view::log_time_view(
		std::string name)
	{
		logger::instance().push_time(name);
	}

	log_time_view::~log_time_view() {
		logger::instance().pop_time();
	}

	void log_time::push_time(
		std::string& name,
		float begin,
		unsigned current_level)
	{
		if (my_level == current_level) {
			my_index++;
			time = begin;
			children.push_back({ name, begin, current_level + 1 });
		}

		else {
			children.at(my_index - 1).push_time(name, begin, current_level);
		}
	}

	void log_time::pop_time(
		float end,
		unsigned current_level)
	{
		if (my_level == current_level) {
			time = end - time;
		}

		else {
			children.at(my_index - 1).pop_time(end, current_level);
		}
	}
#endif
}
