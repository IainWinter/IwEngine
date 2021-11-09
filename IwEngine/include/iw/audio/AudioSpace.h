#pragma once

#include "IwAudio.h"
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
		ENGINE_ALREADY_LOADED   = -5

	};

	class IAudioSpace {
	public:
		std::string RootDirectory;

		IWAUDIO_API
		static IAudioSpace* Create();

		virtual ~IAudioSpace() = default;

		virtual int Initialize() = 0;
		virtual int Update()    = 0;
		virtual int Destroy()   = 0;

		virtual int Load(const std::string& path) = 0;
		virtual int Play(const std::string& path, bool loop, bool stream) = 0;

		virtual int Set(int handle, const std::string& parameter, float  value) = 0;
		virtual int Get(int handle, const std::string& parameter, float& value) = 0;

		virtual int Start(int handle) = 0;
		virtual int Stop (int handle) = 0;
		virtual int Free (int handle) = 0;

		virtual int SetVolume(float  volume) = 0;
		virtual int GetVolume(float& volume) = 0;

	protected:
		int SetHigh(int low, int high) const
		{
			return low + (high << sizeof(int) * 8 / 2);
		}

		virtual bool CheckError(
			int result,
			AudioErrorCode code) = 0;
	};
}

	using namespace Audio;
}
