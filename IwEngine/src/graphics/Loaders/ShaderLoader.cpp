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

		Shader* shader = new Shader();

		size_t index = source.find("#shader");
		while (index < source.size()) {
			size_t start = source.find('\n', index);
			size_t end   = source.find("#shader", start);

			size_t offset = index + 8;
			std::string name = source.substr(offset, start - offset);
			
			index = end;

			ShaderType type;
			if (name == "Vertex") {
				type = VERTEX;
			}

			else if (name == "Fragment") {
				type = FRAGMENT;
			}

			else {
				LOG_WARNING << "Invalid shader type " << name;
				return nullptr;
			}

			std::string code = source.substr(start, end - start);
			shader->AddShader(type, code.c_str());
		}

		return shader;
	}
}
