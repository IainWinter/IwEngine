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
		std::unordered_map<std::string, iwu::ref<_r>> m_loaded;
	protected:
		AssetManager& m_asset;

	private:
		virtual _r* LoadAsset(
			std::string filepath) = 0;

		virtual void FreeAsset(
			_r* asset)
		{}
	public:
		AssetLoader(
			AssetManager& asset)
			: m_asset(asset)
		{}

		~AssetLoader() {
			for (auto r : m_loaded) {
				FreeAsset(r.second.get());
			}
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
			if (!asset) { return; }

			_r* resource = (_r*)malloc(sizeof(_r));
			assert(resource);

			new (resource) _r(std::forward<_r>(*(_r*)asset));

			m_loaded[name] = iwu::ref<_r>(resource);
		}

		virtual void Release(
			iwu::ref<_r> resource)
		{
			for (auto it = m_loaded.begin(); it != m_loaded.end(); it++) {
				if (it->second == resource) {
					FreeAsset(it->second.get());
					m_loaded.erase(it);
					LOG_INFO << "Releasing " << it->first;
					return;
				}
			}

			LOG_WARNING << "Trying to release invalid resource at " << resource;
		}

		virtual void Release(
			std::string name)
		{
			auto it = m_loaded.find(name);
			if (it != m_loaded.end()) {
				FreeAsset(it->second.get());
				m_loaded.erase(it);
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
