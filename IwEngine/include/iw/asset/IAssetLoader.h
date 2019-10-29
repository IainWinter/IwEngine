#pragma once

#include "iw/util/memory/smart_pointers.h"
#include <string>

namespace IW {
inline namespace Asset {
	class IAssetLoader {
	public:
		virtual ~IAssetLoader() {}

		virtual iwu::ref<void> Load(
			std::string filepath) = 0;

		virtual void Give(
			std::string filename,
			void* asset) = 0;
	};
}
}
