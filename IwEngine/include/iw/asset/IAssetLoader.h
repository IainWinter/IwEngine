#pragma once

#include "iw/util/memory/ref.h"
#include <string>

namespace iw {
namespace Asset {
	class IAssetLoader {
	public:
		virtual ~IAssetLoader() {}

		virtual ref<void> Load(
			std::string filepath) = 0;

		virtual ref<void> Give(
			std::string filename,
			void* asset) = 0;

		virtual ref<void> Give(
			std::string name,
			const ref<void>& asset) = 0;

		virtual bool HasLoaded(
			const std::string& name) = 0;
	};
}

	using namespace Asset;
}
