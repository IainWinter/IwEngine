#include "iw/util/io/File.h"
#include <cstdio>
#include <cerrno>
#include <iostream>

#define MAX_LINE_LENGTH 1000

namespace IwUtil {
	static void ReportError(
		int errerno,
		const std::string& info)
	{
		std::string error;
		error.resize(256);

		strerror_s(&error[0], 100, errerno);

		int i = error.find('\0');
		error = error.substr(0, i);
		error.append(" (");
		error.append(std::to_string(errerno));
		error.append("): ");
		error.append(info);

		std::cout << error << std::endl;
	}

	std::string ReadFile(
		const char* filePath)
	{
		std::string contents;
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath, "rt");
		if (file) {
			fseek(file, 0, SEEK_END);
			contents.resize(ftell(file));
			rewind(file);
			fread(&contents[0], 1, contents.size(), file);
			fclose(file);
		}
		else {
			ReportError(errorCode, filePath);
		}

		return contents;
	}

	std::vector<std::string> ReadFileLines(
		const char* filePath)
	{
		std::vector<std::string> lines;
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath, "rt");
		if (file) {
			int lineCount = 0;
			for (char c = getc(file); c != EOF; c = getc(file)) {
				if (c == '\n') lineCount++;
			}

			rewind(file);
			lines.reserve(lineCount + 1);

			char line[MAX_LINE_LENGTH];
			while (fgets(line, MAX_LINE_LENGTH, file)) {
				char& last = line[strlen(line) - 1];
				if (last == '\n') {
					last = '\0';
				}

				lines.push_back(line);
			}
		}
		else {
			ReportError(errorCode, filePath);
		}

		return lines;
	}

	bool FileExists(
		const char* filePath)
	{
		FILE* file;
		fopen_s(&file, filePath, "rb");
		if (file) {
			fclose(file);
		}

		return file;
	}

	uintmax_t GetFileSize(
		const char* filePath)
	{
		uintmax_t bytes;
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath, "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			bytes = ftell(file);
			fclose(file);
		}
		else {
			ReportError(errorCode, filePath);
		}

		return bytes;
	}
}
