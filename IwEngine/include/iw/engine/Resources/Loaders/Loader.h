#pragma once

#include <vector>

namespace IwEngine {
	template<
		typename _resource_t>
	class Loader {
	private:
		std::vector< _resource_t*> m_loaded;
		
	public:
		virtual _resource_t* Load(const char* path) = 0;
		virtual void Release(_resource_t* resource) = 0;
	};
}
