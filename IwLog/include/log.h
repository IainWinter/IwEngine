#pragma once

#include <fstream>
#include "iwlog.h"

namespace iwlog {
	enum loglevel {
		IW_INFO,
		IW_DEBUG,
		IW_WARNING,
		IW_ERROR,
		IW_TRACE
	};

	class IWLOG_API logger {
	public:
	private:
		struct {
			std::ofstream file;
			loglevel min_level;
			bool active;
		};

		static logger instance;
		static const char* level_names[];

	public:
		static void start(
			loglevel min_level,
			const char* file = nullptr);

		static void stop();

		static void write(
			loglevel level,
			const char* message);
	private:
		logger();
		logger(const logger&) = delete;
		logger(logger&&) = delete;
	};
}

#define LOG_START(_min_level_, _file_) iwlog::logger::start(_min_level_, _file_);
#define LOG_STOP() iwlog::logger::stop();

#define LOG(_priority_, _message_) \
	iwlog::logger::write(_priority_, _message_);

#define LOG_INFO(_message_)    LOG(iwlog::IW_INFO,    _message_)
#define LOG_DEBUG(_message_)   LOG(iwlog::IW_DEBUG,   _message_)
#define LOG_WARNING(_message_) LOG(iwlog::IW_WARNING, _message_)
#define LOG_ERROR(_message_)   LOG(iwlog::IW_ERROR,   _message_)
#define LOG_TRACE(_message_)   LOG(iwlog::IW_TRACE,   _message_)
