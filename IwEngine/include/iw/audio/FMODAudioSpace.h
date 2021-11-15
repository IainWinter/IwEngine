#pragma once

#include "AudioSpace.h"
#include "fmod/fmod_studio.hpp"

#undef CreateEvent

#ifdef IW_DEBUG
#	define CHECK_ERROR(stmt, n) if(CheckError(stmt, n)) return n;
#else
#	define CHECK_ERROR(stmt, n) stmt
#endif

#define H_BANK 1
#define H_EVENT 2
#define H_INSTANCE 3
#define H_BUS 4
#define H_VCA 5

#define CHECK_HANDLE(h) if (!IsLoaded(h)) return log(ENGINE_FAILED_HANDLE_NOT_LOADED);

namespace iw
{
namespace Audio
{
	class FMODAudioSpace
		: public IAudioSpace
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
		IWAUDIO_API int Initialize() override;
		IWAUDIO_API int Update()     override;
		IWAUDIO_API int Destroy()    override;

		// Loads a bank and it's event descriptions
		IWAUDIO_API
		int Load(
			const std::string& path) override;

		// if path has event:/ then play an event from a bank, else treat like a url and call playSound
		// return the instance handle
		IWAUDIO_API
		int Play(
			const std::string& path) override;

		// get and set parameters by string

		IWAUDIO_API
		int Set(
			int handle,
			const std::string& parameter,
			float value) override;

		IWAUDIO_API
		int Get(
			int handle,
			const std::string& parameter,
			float& value) override;

		IWAUDIO_API int Start(int handle) override;
		IWAUDIO_API int Stop (int handle) override;
		IWAUDIO_API int Free (int handle) override;

		IWAUDIO_API int SetVolume(int handle, float  volume) override;
		IWAUDIO_API int GetVolume(int handle, float& volume) override;

		IWAUDIO_API bool IsLoaded(int handle) const override;

	private:
		bool CheckError(
			int result,
			AudioErrorCode code) const override;

		// helper functions

		int Put(
			const std::string& path,
			FMOD::Studio::Bank* bank)
		{
			int handle = MakeHandle(H_BANK);
			PutLoaded(path, handle);
			m_banks[handle] = bank;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::EventDescription* event)
		{
			int handle = MakeHandle(H_EVENT);
			PutLoaded(path, handle);
			m_events[handle] = event;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::EventInstance* instance)
		{
			int handle = MakeHandle(H_INSTANCE);
			PutLoaded(path, handle);
			m_instances[handle] = instance;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::Bus* bus)
		{
			int handle = MakeHandle(H_BUS);
			PutLoaded(path, handle);
			m_buses[handle] = bus;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::VCA* vca)
		{
			int handle = MakeHandle(H_VCA);
			PutLoaded(path, handle);
			m_vcas[handle] = vca;
			return handle;
		}

		template<
			typename _t,
			typename _f1,
			typename _f2>
		int LoadFromBank(
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
				_t** items = new _t *[count];

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
}

	using namespace Audio;
}
