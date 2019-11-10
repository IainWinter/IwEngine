#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include <vector>

namespace IW {
inline namespace Graphics {
	enum ShaderType {
		VERTEX,
		GEOMETRY,
		FRAGMENT
	};

	struct IWGRAPHICS_API Shader {
	private:
		struct ShaderSource {
			size_t SourceSize;
			char* Source;
			ShaderType Type;
		};

	public:
		iwu::ref<IPipeline> Handle; // not really sure of the name for Shader Program. Every engine just calls it Shader but thats wrong kinda?
	private:
		std::vector<ShaderSource> m_source;

	public:
		Shader();

		void AddShader(
			ShaderType type,
			const char* source);

		void Initialize(
			const iwu::ref<IDevice>& device);
	};
}
}
