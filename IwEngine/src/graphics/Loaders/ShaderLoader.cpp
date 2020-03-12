#include "iw/graphics/Loaders/ShaderLoader.h"
#include "iw/util/io/File.h"

namespace iw {
namespace Graphics {
	ShaderType TypeFromName(
		std::string& name);

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
			if (index != 0 && start == 0) {
				start = source.find('\0', index) + 1;
			}

			size_t end = source.find("#shader", start);

			size_t offset = index + 8;
			std::string name = source.substr(offset, start - offset - 1);
			
			index = end;

			ShaderType type = TypeFromName(name);

			std::string code;

			if(type == ShaderType::INVALID) {
				std::string include = name.substr(name.find_first_of(' ') + 1, name.find_first_of('\0') - name.find_first_of(' ') - 1);
				            name    = name.substr(0,                           name.find_first_of(' '));

				type = TypeFromName(name);

				LOG_INFO << "\tGetting " << name << " shader source from " << include;
				code = iw::ReadFile("assets/" + include);

				if (type == ShaderType::INVALID) {
					LOG_WARNING << "\tInvalid shader type " << name << " from " << filepath << "@c" << offset;
					return nullptr;
				}
			}

			else {
				code = source.substr(start, end - start);
			}

			// Only make this when we need to
			if (shader == nullptr) {
				shader = new Shader();
			}

			shader->AddShader(type, code.c_str());
		}

		return shader;
	}

	ShaderType TypeFromName(
		std::string& name)
	{
		if (name == "Vertex") {
			return ShaderType::VERTEX;
		}

		else if (name == "Fragment") {
			return ShaderType::FRAGMENT;
		}

		else if (name == "Geometry") {
			return ShaderType::GEOMETRY;
		}

		return ShaderType::INVALID;
	}
}
}
