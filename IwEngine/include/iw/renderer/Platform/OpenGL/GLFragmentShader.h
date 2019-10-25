#pragma once

#include "iw/renderer/FragmentShader.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API GLFragmentShader
		: public IFragmentShader
	{
	private:
		unsigned int m_fragmentShader;

	public:
		GLFragmentShader(
			const char* source);

		~GLFragmentShader();

		inline unsigned int FragmentShader() const {
			return m_fragmentShader;
		}
	};
}
}
