#pragma once

#include "IwAudio.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <vector>
#include <string>

#undef PlaySound

namespace iw {
namespace Audio {

	// this is suppose to be an interface for any audio engine
	// if I wanted to make a toy one later, fmod uses banks of _??__
	// and audio files, not sure how to make an interface without refering
	// to these concepts?

	// would be cool to have a unified resource url type thing
	// instead of filePath for example, this could be a url from
	// the a file, web, or mem address

	// this is a super funcky api
	// trying multiple functions that do dif things based on the type of handle
	// passed to them

	// all calls return a handle or an error code
	// all error codes are negitive

	enum AudioErrorCode : int
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

	// 0 is a special handle representing global actions

	class IAudioSpace {
	public:
		std::string RootDirectory;
		std::unordered_map<std::string, int> Loaded; // name, handle - hashed paths of loaded objects

		IWAUDIO_API
		static IAudioSpace* Create();

		virtual ~IAudioSpace() = default;

		virtual int Initialize() = 0;
		virtual int Update()    = 0;
		virtual int Destroy()   = 0;

		virtual int Load(const std::string& path) = 0;
		virtual int Play(const std::string& path) = 0;

		virtual int Set(int handle, const std::string& parameter, float  value) = 0;
		virtual int Get(int handle, const std::string& parameter, float& value) = 0;

		virtual int Start(int handle) = 0;
		virtual int Stop (int handle) = 0;
		virtual int Free (int handle) = 0;

		virtual int SetVolume(int handle, float  volume) = 0;
		virtual int GetVolume(int handle, float& volume) = 0;

		virtual bool IsLoaded(int handle) const = 0;
		// only virtual so IsLoaded doesnt need
		// to iterate the loaded map's values

		IWAUDIO_API bool IsLoaded (const std::string& path) const;
		IWAUDIO_API int  GetHandle(const std::string& path) const;

	protected:
		virtual bool CheckError(
			int result,
			AudioErrorCode code) const = 0;

		int log(
			AudioErrorCode code) const;

		// helper functions

		int SetType(int high, int low) const
		{
			return low + (high << sizeof(int) * 8 / 2);
		}

		int GetType(int handle) const
		{
			return handle >> (sizeof(int) * 8 / 2);
		}

		int MakeHandle(
			int type) const
		{
			return SetType(type, (int)Loaded.size() + 1);
		}

		void PutLoaded(
			const std::string& path,
			int handle)
		{
			Loaded.emplace(path, handle);
			LOG_INFO << "Audio Engine loaded: " << path;
		}
	};
}

	using namespace Audio;
}
