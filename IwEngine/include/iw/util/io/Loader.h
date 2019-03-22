#pragma once

#include <vector>

namespace IwUtil {
	template<
		typename _resource_t>
		class Loader {
		protected:
			std::vector<_resource_t*> loaded;

		public:
			virtual _resource_t* Load(const char* path) = 0;
			virtual void Release(_resource_t* resource) = 0;
	};
}
