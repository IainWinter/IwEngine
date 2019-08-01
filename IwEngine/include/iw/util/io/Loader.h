#pragma once

#include "iw/util/iwutil.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <assert.h>

namespace iwutil {
	template<
		typename _resource_t>
	class Loader {
		private:
			//Impl some caching by using path (name) and the resources?
			std::unordered_map<const char*, _resource_t*> loaded;

			virtual _resource_t* LoadAsset(
				const char* path) = 0;
		public:
			virtual ~Loader() {
				for (auto r : loaded) {
					delete r.second;
				}
			}

			_resource_t* Load(
				const char* path)
			{
				_resource_t* resource;
				if (loaded.find(path) != loaded.end()) {
					resource = loaded.at(path);
				}

				else {
					resource = LoadAsset(path);
					if (resource == nullptr) {
						LOG_ERROR << "Failed to load resource " << path << "!";
						assert(false);
					}

					loaded.emplace(path, resource);
				}

				return resource;
			}

			virtual void Release(
				_resource_t* resource)
			{
				for (auto it = loaded.begin(); it != loaded.end(); it++) {
					if (it->second == resource) {
						loaded.erase(it);
						LOG_INFO << "Releasing " << it->first;
						return; // Exit function if found
					}
				}

				LOG_WARNING << "Trying to release invalid resource at " << resource;
			}

			virtual void Release(
				const char* name)
			{
				auto it = loaded.find(name);
				if (it != loaded.end()) {
					Release(it->second);
					LOG_INFO << "Releasing " << name;
				}

				else {
					LOG_WARNING << "Trying to release non loaded resource " << name;
				}
			}
	};
}
