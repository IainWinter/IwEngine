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
		std::string source = iw::ReadFile(filepath);

		Shader* shader = nullptr;

		size_t index = source.find("#shader");
		while (index < source.size()) {
			size_t start = source.find('\n', index) + 1;
			size_t end   = source.find("#shader", start);

			size_t offset = index + 8;
			std::string name = source.substr(offset, start - offset - 1);
			
			index = end;

			ShaderType type;
			if (name == "Vertex") {
				type = ShaderType::VERTEX;
			}

			else if (name == "Fragment") {
				type = ShaderType::FRAGMENT;
			}

			else {
				LOG_WARNING << "Invalid shader type " << name << " from " << filepath << "@c" << offset;
				return nullptr;
			}

			std::string code = source.substr(start, end - start);
			
			// Only make this when we need to
			if (shader == nullptr) {
				shader = new Shader();
			}

			shader->AddShader(type, code.c_str());
		}

		return shader;
	}
}
