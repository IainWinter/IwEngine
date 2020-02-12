#include "iw/graphics/Shader.h"
#include "iw/util/enum/val.h"

namespace iw {
namespace Graphics {
	Shader::Shader()
		: Program(nullptr)
	{}

	void Shader::AddShader(
		ShaderType type, 
		const char* source)
	{
		size_t size = strlen(source) + 1;

		ShaderSource shader = {
			size,
			(char*)malloc(size),
			type
		};

		if (!shader.Source) return;
		memcpy(shader.Source, source, size);

		m_source.push_back(shader);
	}

	void Shader::Initialize(
		const iw::ref<IDevice>& device)
	{
		IVertexShader* vertex = nullptr;
		IGeometryShader* geometry = nullptr;
		IFragmentShader* fragment = nullptr;

		for (ShaderSource& shader : m_source) {
			switch (shader.Type) {
				case ShaderType::VERTEX:   vertex   = device->CreateVertexShader(shader.Source);   break;
				case ShaderType::GEOMETRY: geometry = device->CreateGeometryShader(shader.Source); break;
				case ShaderType::FRAGMENT: fragment = device->CreateFragmentShader(shader.Source); break;
				default: LOG_WARNING << "Invalid shader type " << iw::val(shader.Type); break;
			}
		}

		Program = iw::ref<IPipeline>(device->CreatePipeline(vertex, fragment, geometry));
	}
}
}
