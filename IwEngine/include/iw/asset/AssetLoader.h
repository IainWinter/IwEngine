#pragma once

#include "IwAsset.h"
#include "IAssetLoader.h"
#include "AssetManager.h"
#include "iw/util/memory/ref.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <assert.h>

namespace iw {
namespace Asset {
	template<
		typename _a>
	class AssetLoader
		: public IAssetLoader
	{
	private:
		std::unordered_map<std::string, iw::ref<_a>> m_loaded;
	protected:
		AssetManager& m_asset;

	private:
		virtual _a* LoadAsset(
			std::string filepath) = 0;

		virtual void FreeAsset(
			_a* asset)
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

		iw::ref<void> Load(
			std::string filepath) override
		{
			_a* asset;
			if (m_loaded.find(filepath) == m_loaded.end()) {
				LOG_INFO << "Loading asset from " << filepath << "...";
				asset = LoadAsset(filepath);
				if (asset == nullptr) {
					LOG_ERROR << "Failed to load asset from " << filepath << "!";
					return nullptr;
				}

				else {
					LOG_INFO << "Done";
				}

				m_loaded.emplace(filepath, asset);
			}

			return m_loaded.at(filepath);
		}

		iw::ref<void> Give(
			std::string name,
			void* asset)
		{
			_a* buf = (_a*)malloc(sizeof(_a));
			assert(buf);

			new (buf) _a(std::forward<_a>(*(_a*)asset));
			m_loaded[name] = iw::ref<_a>(buf);

			return m_loaded.at(name);
		}

		iw::ref<void> Give(
			std::string name,
			const ref<void>& asset)
		{
			m_loaded[name] = std::static_pointer_cast<_a, void>(asset);
			return m_loaded.at(name);
		}

		virtual void Release(
			iw::ref<_a> resource)
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
			return typeid(_a).hash_code();
		}
	};
}

	using namespace Asset;
}
