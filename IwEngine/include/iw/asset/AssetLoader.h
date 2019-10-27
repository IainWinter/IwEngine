#pragma once

#include "IwAsset.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <assert.h>

namespace IW {
	inline namespace Asset {
		class IAssetLoader {
		public:
			//virtual po LoadAsync()

			virtual ~IAssetLoader() {}

			virtual size_t GetType() = 0;

			virtual void* Load(
				const char* filepath) = 0;
		};

		template<
			typename _resource_t>
		class AssetLoader
			: public IAssetLoader
		{
		private:
			//Impl some caching by using path (name) and the resources?
			std::unordered_map<const char*, _resource_t*> m_loaded;

			virtual _resource_t* LoadAsset(
				const char* filepath) = 0;
		public:
			~AssetLoader() {
				for (auto r : m_loaded) {
					delete r.second;
				}
			}

			size_t GetType() override {
				return typeid(_resource_t).hash_code();
			}

			void* Load(
				const char* filepath) override
			{
				_resource_t* resource;
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

			virtual void Release(
				_resource_t* resource)
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
