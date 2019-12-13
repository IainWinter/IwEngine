#pragma once

#include "iw/util/memory/smart_pointers.h"
#include <string>

namespace IW {
namespace Asset {
	class IAssetLoader {
	public:
		virtual ~IAssetLoader() {}

		virtual iw::ref<void> Load(
			std::string filepath) = 0;

		virtual iw::ref<void> Give(
			std::string filename,
			void* asset) = 0;
	};
}

	using namespace Asset;
}
