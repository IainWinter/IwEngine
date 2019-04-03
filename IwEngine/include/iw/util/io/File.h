#pragma once

#include "iw./util/iwutil.h"
#include <string>
#include <vector>

namespace iwutil {
	std::string ReadFile(
		const char* filePath);

	std::vector<std::string> ReadFileLines(
		const char* filePath);

	bool FileExists(
		const char* filePath);

	uintmax_t GetFileSize(
		const char* filePath);
}
