#pragma once

#include "iw/util/memory/ref.h"
#include <string>

namespace iw {
namespace Asset {
	class IAssetLoader {
	public:
		virtual ~IAssetLoader() {}

		virtual iw::ref<void> Load(
			std::string filepath) = 0;

		virtual iw::ref<void> Give(
			std::string filename,
			void* asset) = 0;

		virtual iw::ref<void> Give(
			std::string name,
			const ref<void>& asset) = 0;
	};
}

	using namespace Asset;
}
