#pragma once

#include "iw./util/iwutil.h"
#include <vector>

namespace iwutil {
	template<
		typename _resource_t>
	class IWUTIL_API Loader {
		protected:
			struct {
				std::vector<_resource_t*> loaded;
			};
		public:
			virtual ~Loader() {
				for (_resource_t* r : loaded) {
					delete r;
				}
			}

			virtual _resource_t* Load(
				const char* path) = 0;

			virtual void Release(
				_resource_t* resource) = 0;
	};
}
