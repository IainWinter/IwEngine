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

	struct IWGRAPHICS_API Pipeline {
	private:
		struct Shader {
			size_t SourceSize;
			char* Source;
			ShaderType Type;
		};

	public:
		iwu::ref<IPipeline> Handle;
	private:
		std::vector<Shader> m_shaders;

	public:
		Pipeline();

		void AddShader(
			ShaderType type,
			const char* source);

		void Initialize(
			const iwu::ref<IDevice>& device);
	};
}
}
