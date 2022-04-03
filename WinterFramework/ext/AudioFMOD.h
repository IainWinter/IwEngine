#pragma once

#include "Audio.h"
//#include "fmod/fmod_studio.hpp"
#include "../IwEngine/extern/fmod/include/fmod/fmod_studio.h"

#undef CreateEvent

#ifdef IW_DEBUG
#	define CHECK_ERROR(stmt, n) if(check_error(stmt, n)) return n;
#else
#	define CHECK_ERROR(stmt, n) stmt
#endif

#define H_BANK 1
#define H_EVENT 2
#define H_INSTANCE 3
#define H_BUS 4
#define H_VCA 5

#define CHECK_HANDLE(h) if (!is_loaded(h)) return log(ENGINE_FAILED_HANDLE_NOT_LOADED);

bool fmod_check_error(
	int result,
	audio_error code) const; 

struct audio_manager_fmod
	: audio_manager
{
private:
	FMOD::Studio::System* m_system;

	std::unordered_map<int, FMOD::Studio::Bank*>             m_banks;
	std::unordered_map<int, FMOD::Studio::Bus*>              m_buses;
	std::unordered_map<int, FMOD::Studio::VCA*>              m_vcas;
	std::unordered_map<int, FMOD::Studio::EventDescription*> m_events;
	std::unordered_map<int, FMOD::Studio::EventInstance*>    m_instances;

	using InstanceUserData = std::pair<int, FMODAudioSpace*>;

public:
	IWAUDIO_API int initialize() override;
	IWAUDIO_API int update()     override;
	IWAUDIO_API int destroy()    override;

	// Load: Loads a bank and its event descriptions
	// Play: If path has event:/ then play an event from a bank, else treat like a url

	IWAUDIO_API int load(const std::string& path) override;
	IWAUDIO_API int play(const std::string& path) override;

	IWAUDIO_API int set(int handle, const std::string& parameter, float  value) override;
	IWAUDIO_API int get(int handle, const std::string& parameter, float& value) override;

	IWAUDIO_API int start(int handle) override;
	IWAUDIO_API int stop (int handle) override;
	IWAUDIO_API int free (int handle) override;

	IWAUDIO_API int set_volume(int handle, float  volume) override;
	IWAUDIO_API int get_volume(int handle, float& volume) override;

	IWAUDIO_API bool is_loaded(int handle) const override;

private:
	int put_loaded(const std::string& path, FMOD::Studio::Bank* bank);
	int put_loaded(const std::string& path, FMOD::Studio::EventDescription* event);
	int put_loaded(const std::string& path, FMOD::Studio::EventInstance* instance);
	int put_loaded(const std::string& path, FMOD::Studio::Bus* bus);
	int put_loaded(const std::string& path, FMOD::Studio::VCA* vca);

	template<
		typename _t,
		typename _f1,
		typename _f2>
	int load_from_bank(
		FMOD::Studio::Bank* bank,
		_f1&& getCount,
		_f2&& getItems)
	{
		int count;
		CHECK_ERROR(
			getCount(bank, count),
			ENGINE_FAILED_LOAD_BANK
		);

		if (count > 0)
		{
			_t** items = new _t*[count];

			CHECK_ERROR(
				getItems(bank, items, count),
				ENGINE_FAILED_LOAD_BANK
			);

			for (int i = 0; i < count; i++)
			{
				std::string name(1024, '\0');
				int size;

				CHECK_ERROR(
					items[i]->getPath(name.data(), 1024, &size),
					ENGINE_FAILED_LOAD_BANK
				);

				name.resize(size - 1);

				Put(name, items[i]);
			}

			delete[] items;
		}

		return ENGINE_OK;
	}
};
