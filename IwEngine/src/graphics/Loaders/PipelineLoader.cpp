#include "iw/graphics/Loaders/PipelineLoader.h"
#include "iw/util/io/File.h"

namespace IW {
	Pipeline* PipelineLoader::LoadAsset(
		std::string filepath)
	{
		std::string source = iwu::ReadFile(filepath);

		std::vector<std::string> lines = iwu::ReadFileLines(filepath);


		//Pipeline* pipeline = new Pipeline();

		//pipeline->AddShader();

		return nullptr;
	}
}
