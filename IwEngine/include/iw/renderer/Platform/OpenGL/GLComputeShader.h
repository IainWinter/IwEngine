#pragma once

#include "iw/renderer/ComputeShader.h"

namespace iw {
namespace RenderAPI {
	class GLComputeShader
		: public IComputeShader
	{
	private:
		unsigned gl_id;

	public:
		IWRENDERER_API
		GLComputeShader(
			const char* source);

		IWRENDERER_API
		unsigned ComputeShader() const;
	};
}

	using namespace RenderAPI;
}
