#include "iw/graphics/Loaders/MaterialLoader.h"
#include "iw/util/io/File.h"

namespace iw {
namespace Graphics {
	MaterialLoader::MaterialLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	Material* MaterialLoader::LoadAsset(
		std::string filepath)
	{
		LOG_INFO << "Loading " << filepath;
		for (std::string line : iw::ReadFileLines(filepath.c_str())) {
			LOG_INFO << line;
		}

		return nullptr;
	}
}
}
