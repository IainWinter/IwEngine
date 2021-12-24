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
		const std::string& name)
	{
		m_root.push_time(name, m_get_time(), m_current_level);
		m_current_level++;
	}

	void logger::pop_time() {
		m_root.pop_time(m_get_time(), m_current_level);

		if (m_current_level > 0) // final pop is at level 0, ends a frame, lil funcky
		{
			m_current_level--;
		}
	}

	log_time logger::get_times() {
		return m_root;
	}

	void logger::clear_times() {
		m_root = {};
	}
#endif
#ifdef IW_LOG_VALUE

	void log::logger::push_value(
		const std::string& name, 
		float value)
	{
		m_values[name].push_back(value);
	}

	const std::vector<float>& log::logger::get_values(
		const std::string& name)
	{
		return m_values[name];
	}

	void log::logger::clear_values()
	{
		m_values.clear();
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

#ifdef IW_LOG_VALUE
		clear_values();
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
		const std::string& name)
	{
		logger::instance().push_time(name);
	}

	log_time_view::~log_time_view() {
		logger::instance().pop_time();
	}

	void log_time::push_time(
		const std::string& name,
		float begin,
		unsigned level)
	{
		if (level == 0) {
			if (children.size() == 0) {
				time = begin;
			}

			children.push_back({ name, begin });
		}
		else {
			children.back().push_time(name, begin, level - 1);
		}
	}

	void log_time::pop_time(
		float end,
		unsigned level)
	{
		if (level == 0) {
			time = end - time;
		} 
		else {
			children.back().pop_time(end, level - 1);
		}
	}
#endif
}
