#include "iw/graphics/Shader.h"
#include "iw/util/enum/val.h"

namespace iw {
namespace Graphics {
	Shader::Shader()
		: m_handle(nullptr)
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

	IPipeline* Shader::Handle() const {
		return m_handle;
	}

	bool Shader::IsInitialized() const {
		return !!m_handle;
	}

	void Shader::Initialize(
		const iw::ref<IDevice>& device)
	{
		if (IsInitialized()) {
			LOG_WARNING << "Tried to initialize a shader twice!";
			return;
		}

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

		m_handle = device->CreatePipeline(vertex, fragment, geometry);
	}

	void Shader::Use(
		const iw::ref<IDevice>& device) const
	{
		if (!IsInitialized()) {
			LOG_WARNING << "Tried to use a shader before it was initialized!";
			return;
		}

		device->SetPipeline(m_handle);
	}
}
}
