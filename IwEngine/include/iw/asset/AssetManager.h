#pragma once

#include "IwAsset.h"
#include "IAssetLoader.h"
#include "iw/util/queue/blocking_queue.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <thread>

namespace iw {
// Have the asset manager store laoding things, just give ref to loaders to load data into them
// 

namespace Asset {
	class AssetManager {
	private:
		std::unordered_map<size_t, IAssetLoader*> m_loaders;
		//iw::blocking_queue<std::pair<size_t, std::string>> m_toLoad;
		//std::vector<std::thread> m_threads;

		//void Worker() {
		//	while (true) {
		//		auto toLoad = m_toLoad.pop();
		//		m_loaders.at(toLoad.first)->Load(toLoad.second);

		//	}
		//}

	public:
		AssetManager() {}

		~AssetManager() {
			for (auto pair : m_loaders) {
				delete pair.second;
			}
		}

		template<
			typename _l,
			typename... _args>
		void SetLoader(
			_args&&... args)
		{
			_l*    loader = new _l(*this, args...);
			size_t type   = _l::GetType();

			auto itr = m_loaders.find(type);
			if (itr != m_loaders.end()) {
				delete itr->second;
				itr->second = loader;
			}

			else {
				m_loaders.emplace(type, loader);
			}
		}

		template<
			typename _a>
		iw::ref<_a> Load(
			std::string filepath)
		{
			auto itr = m_loaders.find(typeid(_a).hash_code());
			if (itr != m_loaders.end()) {
				if (filepath.find_first_of(':') > 3) { // only if not A:\ or AA:\ or AAA:\ 
					filepath = "assets/" + filepath;
				}

				return std::static_pointer_cast<_a, void>(itr->second->Load(filepath));
			}

			return nullptr;
		}

		template<
			typename _a>
		iw::ref<_a> Give(
			std::string name,
			_a* asset)
		{
			auto itr = m_loaders.find(typeid(_a).hash_code());
			if (itr != m_loaders.end()) {
				return std::static_pointer_cast<_a, void>(itr->second->Give("assets/" + name, asset));
			}

			return nullptr;
		}

		template<
			typename _a>
		iw::ref<_a> Give(
			std::string name,
			const ref<_a>& asset)
		{
			auto itr = m_loaders.find(typeid(_a).hash_code());
			if (itr != m_loaders.end()) {
				return  std::static_pointer_cast<_a, void>(itr->second->Give("assets/" + name, asset));
			}

			return nullptr;
		}

		template<
			typename _a>
		bool HasLoaded(
			const std::string& name)
		{
			auto itr = m_loaders.find(typeid(_a).hash_code());
			return itr->second->HasLoaded(name);
		}
	};
}

	using namespace Asset;
}
