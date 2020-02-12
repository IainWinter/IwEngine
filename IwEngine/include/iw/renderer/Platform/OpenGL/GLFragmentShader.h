#pragma once

#include "iw/renderer/FragmentShader.h"

namespace iw {
namespace RenderAPI {
	class GLFragmentShader
		: public IFragmentShader
	{
	private:
		unsigned gl_id;

	public:
		IWRENDERER_API
		GLFragmentShader(
			const char* source);

		IWRENDERER_API
		~GLFragmentShader() = default;

		IWRENDERER_API
		unsigned FragmentShader() const;
	};
}

	using namespace RenderAPI;
}
