#pragma once

#include <string>
#include <vector>

namespace IwUtil {
	std::string ReadFile(
		const char* filePath);

	std::vector<std::string> ReadFileLines(
		const char* filePath);

	bool FileExists(
		const char* filePath);

	uintmax_t GetFileSize(
		const char* filePath);
}
