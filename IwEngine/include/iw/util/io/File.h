#pragma once

#include "iw/util/iwutil.h"
#include <string>
#include <vector>

namespace iwutil {
	IWUTIL_API std::string ReadFile(
		const char* filePath);

	IWUTIL_API std::vector<std::string> ReadFileLines(
		const char* filePath);

	IWUTIL_API bool FileExists(
		const char* filePath);

	IWUTIL_API uintmax_t GetFileSize(
		const char* filePath);
}
