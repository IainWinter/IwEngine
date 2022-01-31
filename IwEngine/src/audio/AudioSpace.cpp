#include "iw/audio/AudioSpace.h"

namespace iw {
namespace Audio
{
	bool IAudioSpace::IsLoaded(
		const std::string& path) const
	{
		return Loaded.find(path) != Loaded.end();
	}

	int IAudioSpace::GetHandle(
		const std::string& path) const 
	{
		auto itr = Loaded.find(path);
		if (itr == Loaded.end())
		{
			return log(ENGINE_FAILED_HANDLE_NOT_LOADED);
		}

		return itr->second;
	}

	int IAudioSpace::log(
		AudioErrorCode code) const
	{
		static std::unordered_map<int, std::string> translation;
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

		LOG_ERROR << "Audio Engine message: " << translation.at(code);

		return code;
	}
}
}
