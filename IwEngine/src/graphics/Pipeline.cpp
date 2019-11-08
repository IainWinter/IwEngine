#include "iw/graphics/Pipeline.h"

namespace IW {
	Pipeline::Pipeline()
		: Handle(nullptr)
	{}

	void Pipeline::AddShader(
		ShaderType type, 
		const char* source)
	{
		size_t size = strlen(source) + 1;

		Shader shader = {
			size,
			(char*)malloc(size),
			type
		};

		memcpy(shader.Source, source, size);

		m_shaders.push_back(shader);
	}

	void Pipeline::Initialize(
		const iwu::ref<IDevice>& device)
	{
		IVertexShader* vertex = nullptr;
		IGeometryShader* geometry = nullptr;
		IFragmentShader* fragment = nullptr;

		for (Shader& shader : m_shaders) {
			switch (shader.Type) {
				case VERTEX:   vertex   = device->CreateVertexShader(shader.Source);   break;
				case GEOMETRY: geometry = device->CreateGeometryShader(shader.Source); break;
				case FRAGMENT: fragment = device->CreateFragmentShader(shader.Source); break;
				default: LOG_WARNING << "Invalid shader type " << shader.Type; break;
			}
		}

		Handle = iwu::ref<IPipeline>(device->CreatePipeline(vertex, fragment, geometry));
	}
}

