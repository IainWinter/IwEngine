#pragma once

#include "IwAsset.h"
#include "IAssetLoader.h"
#include "AssetManager.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <string>
#include <assert.h>

namespace IW {
inline namespace Asset {
	template<
		typename _r>
	class AssetLoader
		: public IAssetLoader
	{
	private:
		std::unordered_map<std::string, iwu::ref<_r>> m_loaded;
	protected:
		AssetManager& m_asset;

	private:
		virtual _r* LoadAsset(
			std::string filepath) = 0;
	public:
		AssetLoader(
			AssetManager& asset)
			: m_asset(asset)
		{}

		~AssetLoader() {
			//for (auto r : m_loaded) {
			//	// call unload or something, might need that?
			//}
		}

		iwu::ref<void> Load(
			std::string filepath) override
		{
			_r* resource;
			if (m_loaded.find(filepath) == m_loaded.end()) {
				resource = LoadAsset(filepath);
				if (resource == nullptr) {
					LOG_ERROR << "Failed to load resource " << filepath << "!";
					assert(false);
				}

				m_loaded.emplace(filepath, resource);
			}

			return m_loaded.at(filepath);
		}

		void Give(
			std::string name,
			void* asset)
		{
			_r* resource = (_r*)malloc(sizeof(_r));

			memcpy(resource, asset, sizeof(_r));
			memset(asset, 0, sizeof(_r));

			m_loaded[name] = iwu::ref<_r>(resource);
		}

		virtual void Release(
			iwu::ref<_r> resource)
		{
			for (auto it = m_loaded.begin(); it != m_loaded.end(); it++) {
				if (it->second == resource) {
					m_loaded.erase(it);
					LOG_INFO << "Releasing " << it->first;
					return; // Exit function if found
				}
			}

			LOG_WARNING << "Trying to release invalid resource at " << resource;
		}

		virtual void Release(
			const char* name)
		{
			auto it = m_loaded.find(name);
			if (it != m_loaded.end()) {
				Release(it->second);
				LOG_INFO << "Releasing " << name;
			}

			else {
				LOG_WARNING << "Trying to release non loaded resource " << name;
			}
		}

		static size_t GetType() {
			return typeid(_r).hash_code();
		}
	};
}
}