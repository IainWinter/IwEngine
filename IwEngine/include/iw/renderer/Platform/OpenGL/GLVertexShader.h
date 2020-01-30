#pragma once

#include "iw/renderer/VertexShader.h"

namespace IW {
namespace RenderAPI {
	class GLVertexShader
		: public IVertexShader
	{
	private:
		unsigned gl_id;

	public:
		IWRENDERER_API
		GLVertexShader(
			const char* source);

		IWRENDERER_API
		~GLVertexShader() = default;

		IWRENDERER_API
		unsigned VertexShader() const;
	};
}

	using namespace RenderAPI;
}
