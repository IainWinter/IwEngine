#include "iw/graphics/Loaders/ShaderLoader.h"
#include "iw/util/io/File.h"

namespace IW {
	ShaderLoader::ShaderLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	Shader* ShaderLoader::LoadAsset(
		std::string filepath)
	{
		std::string source = iwu::ReadFile(filepath);

		std::vector<std::string> lines = iwu::ReadFileLines(filepath);


		//Pipeline* pipeline = new Pipeline();

		//pipeline->AddShader();

		return nullptr;
	}
}
