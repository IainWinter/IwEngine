#ifndef MINILOG_H_
#define MINILOG_H_

#include <stdio.h>
#include <sstream>
#include <string>

enum minilog_level {
	logNONE,
	logERROR,
	logWARNING,
	logINFO,
	logDEBUG,
	logTRACE,
	minilog_level_max
};


// minilog_get_timestr() for Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

inline std::string
minilog_get_timestr()
{
	const int MAX_LEN = 100;
	char buffer[MAX_LEN];

	if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
		"HH':'mm':'ss", buffer, MAX_LEN) == 0) {
		return "";
	}

	char result[MAX_LEN * 2] = { 0 };
	static DWORD first = GetTickCount();
	sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);

	return result;
}

// get_timestr() for Linux, OSX
#else

#include <sys/time.h>

inline std::string
minilog_get_timestr()
{
	const int MAX_LEN = 100;
	char buffer[MAX_LEN];

	time_t t;
	time(&t);
	tm r = { 0 };
	strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));

	char result[MAX_LEN * 2] = { 0 };
	struct timeval tv;
	gettimeofday(&tv, 0);
	sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);

	return result;
}
#endif // minilog_get_timestr()


template <typename T>
class Log
{
public:
	Log();

	virtual
		~Log();

	std::ostringstream&
		get(
			minilog_level level);

	static minilog_level&
		current_level();

	static std::string
		to_string(
			minilog_level level);

	static minilog_level
		from_string(
			const std::string& level);

protected:
	std::ostringstream os;

private:
	Log(
		const Log&);

	Log&
		operator =(
			const Log&);
};

template <typename T>
Log<T>::Log() {
}

template <typename T>
std::ostringstream& Log<T>::get(
	minilog_level level) {
	os << minilog_get_timestr() << " " << to_string(level) << ": ";
	return os;
}

template <typename T>
Log<T>::~Log() {
	os << std::endl;
	T::write(os.str());
}

template <typename T>
minilog_level&
Log<T>::current_level() {
	static minilog_level current_level = logNONE;
	return current_level;
}

template <typename T>
std::string
Log<T>::to_string(
	minilog_level level) {
	static const char* const buffer[] = {
		   "NONE",
		   "ERROR",
		   "WARNING",
		   "INFO",
		   "DEBUG",
		   "TRACE" };
	return buffer[level];
}

template <typename T>
minilog_level
Log<T>::from_string(
	const std::string & level) {
	if (level == "TRACE")
		return logTRACE;
	else if (level == "DEBUG")
		return logDEBUG;
	else if (level == "INFO")
		return logINFO;
	else if (level == "WARNING")
		return logWARNING;
	else if (level == "ERROR")
		return logERROR;
	else if (level == "NONE")
		return logNONE;
	else {
		Log<T>().Get(logWARNING) << "Unknown logging level '" << level
			<< "'. Using INFO level as default.";
		return logINFO;
	}
}

class FileLogger
{
public:
	static FILE*&
		get_stream();

	static void
		write(
			const std::string& msg);
};

inline FILE*&
FileLogger::get_stream() {
	static FILE* stream = stderr;
	return stream;
}

inline void
FileLogger::write(
	const std::string & msg) {

	FILE* stream = get_stream();
	if (!stream) {
		return;
	}
	fprintf(stream, "%s", msg.c_str());
	fflush(stream);
}

class MiniLog :
	public Log<FileLogger>
{
};

#define MINILOG(level) \
    if (level > MiniLog::current_level() || !FileLogger::get_stream()) ; \
    else MiniLog().get(level)


#endif /* MINILOG_H_ */
