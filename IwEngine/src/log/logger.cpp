#include "iw/log/logger.h"

namespace iwlog {
	static std::string tags[5] = {
		"[Info ] ",
		"[Debug] ",
		"[Warn ] ",
		"[Error] ",
		"[Trace] "
	};

	logger& logger::instance() {
		static logger log;
		return log;
	}

	void logger::sink_msg(
		loglevel level,
		std::string string)
	{
		for (sink* sink : m_sinks) {
			if (sink->should_log(level)) {
				sink->log(string);
			}
		}
	}

	void logger::flush() {
		for (sink* sink : m_sinks) {
			sink->flush();
		}
	}

	log_view::log_view(
		loglevel level)
		: m_level(level)
	{
		logger::instance().log(m_level, "\n");
		logger::instance().log(m_level, tags[level]);
	}
}
