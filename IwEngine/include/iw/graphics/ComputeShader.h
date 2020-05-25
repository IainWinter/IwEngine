#pragma once

#include "Shader.h"

namespace iw {
namespace Graphics {
	struct ComputeShader
		: Shader
	{
	public:
		IWGRAPHICS_API
		inline void AddShader(
			const char* source)
		{
			Shader::AddShader(ShaderType::COMPUTE, source);
		}

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device) override;
	};
}

	using namespace Graphics;
}
