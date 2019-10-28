#pragma once

#include "IwAsset.h"
#include "IAssetLoader.h"
#include "AssetManager.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <assert.h>

namespace IW {
inline namespace Asset {
	template<
		typename _r>
	class AssetLoader
		: public IAssetLoader
	{
	private:
		std::unordered_map<const char*, _r*> m_loaded;
	protected:
		AssetManager& m_asset;

	private:
		virtual _r* LoadAsset(
			const char* filepath) = 0;
	public:
		AssetLoader(
			AssetManager& asset)
			: m_asset(asset)
		{}

		~AssetLoader() {
			for (auto r : m_loaded) {
				delete r.second;
			}
		}

		size_t GetType() override {
			return typeid(_r).hash_code();
		}

		void* Load(
			const char* filepath) override
		{
			_r* resource;
			if (m_loaded.find(filepath) != m_loaded.end()) {
				resource = m_loaded.at(filepath);
			}

			else {
				resource = LoadAsset(filepath);
				if (resource == nullptr) {
					LOG_ERROR << "Failed to load resource " << filepath << "!";
					assert(false);
				}

				m_loaded.emplace(filepath, resource);
			}

			return resource;
		}

		void Give(
			const char* name,
			void* asset)
		{
			_r* resource = (_r*)malloc(sizeof(_r));
			memmove(resource, asset, sizeof(_r));
			m_loaded[name] = resource;
		}

		virtual void Release(
			_r* resource)
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
	};
}
}
