#include "log.h"
#include <iostream>

namespace iwlog {
	logger logger::instance;

	const char* logger::level_names[5] = {
		"Info",
		"Debug",
		"Warning",
		"Error",
		"Trace"
	};

	logger::logger()
		: active(false)
	{}

	void logger::start(
		loglevel min_level,
		const char* file)
	{
		instance.active = true;
		instance.min_level = min_level;
		if (file) {
			instance.file.open(file);
		}
	}

	void logger::stop() {
		instance.active = false;
		if (instance.file.is_open()) {
			instance.file.close();
		}
	}

	void logger::write(
		loglevel level,
		const char* message)
	{
		if (instance.active && level >= instance.min_level) {
			std::ostream& stream = instance.file.is_open() 
				? instance.file : std::cout;

			stream << "[" << level_names[level] << "] "
				<< message << std::endl;
		}
	}
}
