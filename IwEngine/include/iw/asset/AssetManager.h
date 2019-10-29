#pragma once

#include "IwAsset.h"
#include "IAssetLoader.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace IW {
	inline namespace Asset {
		class AssetManager {
		private:
			std::unordered_map<size_t, IAssetLoader*> m_loaders;

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
				typename _t>
			iwu::ref<_t> Load(
				std::string filepath)
			{
				auto itr = m_loaders.find(typeid(_t).hash_code());
				if (itr != m_loaders.end()) {
					return std::static_pointer_cast<_t, void>(itr->second->Load(filepath));
				}

				return nullptr;
			}

			template<
				typename _t>
			void Give(
				std::string name,
				_t* asset)
			{
				auto itr = m_loaders.find(typeid(_t).hash_code());
				if (itr != m_loaders.end()) {
					itr->second->Give(name, asset);
				}
			}
		};
	}
}
