#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWAUDIO_DLL
#		define IWAUDIO_API __declspec(dllexport)
#else
#		define IWAUDIO_API __declspec(dllimport)
#	endif
#else
#	define IWAUDIO_API
#endif


#include <unordered_map>
#include <vector>
#include <string>
#include <stdio.h>

#undef PlaySound

// all calls return a handle or an error code
// all error codes are negitive

constexpr int audio_set_type(int high, int low) { return low + (high << sizeof(int) * 8 / 2); }
constexpr int audio_get_type(int handle) { return handle >> (sizeof(int) * 8 / 2); }
constexpr int audio_make_handle(int type, int index) {	return audio_set_type(type, index); }

enum audio_error : int
{
	ENGINE_OK               =  0,
	ENGINE_FAILED_CREATE    = -1,
	ENGINE_FAILED_INIT      = -2,
	ENGINE_FAILED_UPDATE    = -3,
	ENGINE_FAILED_LOAD_BANK = -4,
	ENGINE_ALREADY_LOADED   = -5,

	ENGINE_FAILED_MAKE_INSTANCE = -6,
	ENGINE_FAILED_LOAD_INSTANCE = -7,
	ENGINE_FAILED_INVALID_HANDLE = -8,
	ENGINE_FAILED_HANDLE_NOT_LOADED = -9,
	ENGINE_FAILED_SET_PARAM = -10,
	ENGINE_FAILED_GET_PARAM = -11
};

const char* audio_error_string(
	audio_error code)
{
	static std::unordered_map<int, const char*> translation;
	if (translation.size() == 0)
	{
		translation.emplace(ENGINE_OK,               "Engine is ok");
		translation.emplace(ENGINE_FAILED_CREATE,    "Engine failed to create fmod system");
		translation.emplace(ENGINE_FAILED_INIT,      "Engine failed to init fmod system");
		translation.emplace(ENGINE_FAILED_UPDATE,    "Engine failed to update fmod system");
		translation.emplace(ENGINE_FAILED_LOAD_BANK, "Engine failed to load a bank");
		translation.emplace(ENGINE_ALREADY_LOADED,   "Engine tried to load a bank that was already loaded");

		translation.emplace(ENGINE_FAILED_MAKE_INSTANCE,     "Engine failed to create an instance");
		translation.emplace(ENGINE_FAILED_LOAD_INSTANCE,     "Engine failed to load an instance");
		translation.emplace(ENGINE_FAILED_INVALID_HANDLE,    "Engine was passed a handle was invalid");
		translation.emplace(ENGINE_FAILED_HANDLE_NOT_LOADED, "Engine was passed a handle that hasn't been loaded");
		translation.emplace(ENGINE_FAILED_SET_PARAM,         "Engine failed to set an instance parameter");
		translation.emplace(ENGINE_FAILED_GET_PARAM,         "Engine failed to get an instance parameter");
	}

	return translation.at(code);
}

void audio_log_error(
	audio_error error)
{
	printf("[Audio] %s", audio_error_string(error));
}

// 0 is a special handle representing global actions

struct audio_manager
{
	std::string m_root_path;
	std::unordered_map<std::string, int> m_loaded; // name, handle - hashed paths of loaded objects

	virtual ~audio_manager() = default;

	virtual int initialize() = 0;
	virtual int update()    = 0;
	virtual int destroy()   = 0;

	virtual int load(const std::string& path) = 0;
	virtual int play(const std::string& path) = 0;

	virtual int set(int handle, const std::string& parameter, float  value) = 0;
	virtual int set(int handle, const std::string& parameter, float& value) = 0;

	virtual int start(int handle) = 0;
	virtual int stop (int handle) = 0;
	virtual int free (int handle) = 0;

	virtual int set_volume(int handle, float  volume) = 0;
	virtual int get_volume(int handle, float& volume) = 0;

	virtual bool is_loaded(int handle) const = 0;

	bool is_loaded(
		const std::string& path) const
	{
		return m_loaded.find(path) != m_loaded.end();
	}

	int get_handle(
		const std::string& path) const
	{
		auto itr = m_loaded.find(path);
		if (itr == m_loaded.end())
		{
			return log(ENGINE_FAILED_HANDLE_NOT_LOADED);
		}

		return itr->second;
	}

protected:
	virtual bool get_error(int result, audio_error code) const = 0;

	void put_loaded(
		const std::string& path,
		int handle)
	{
		m_loaded.emplace(path, handle);
		printf("[Audio] Engine loaded: %s. Handle: %d", path.c_str(), handle);
	}
};
