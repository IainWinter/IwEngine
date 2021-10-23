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
	protected:
		AssetManager& m_asset;
		std::unordered_map<std::string, iw::ref<_a>> m_loaded;

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
			if (m_loaded.find(filepath) == m_loaded.end())
			{
				LOG_INFO << "Loading asset from " << filepath << "...";
				asset = LoadAsset(filepath);
				if (asset == nullptr)
				{
					LOG_ERROR << "\tFailed to load asset from " << filepath << "!";
					return nullptr;
				}

				else
				{
					LOG_INFO << "\tDone";
				}

				m_loaded.emplace(filepath, asset);
			}

			return m_loaded.at(filepath);
		}

		std::string Release(
			const ref<void>& loaded) override
		{
			for (auto& [path, asset] : m_loaded)
			{
				if (asset == loaded)
				{
					LOG_INFO << "Unloading asset " << path << "...";
					std::string ret = path;

					FreeAsset(asset.get());
					m_loaded.erase(path);

					return ret;
				}
			}

			LOG_WARNING << "Tried to release asset that doesnt exist! " << (char*)loaded.get();

			return "";
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

		bool HasLoaded(
			const std::string& name)
		{
			return m_loaded.find(name) != m_loaded.end();
		}

		static size_t GetType() {
			return typeid(_a).hash_code();
		}
	};
}

	using namespace Asset;
}
