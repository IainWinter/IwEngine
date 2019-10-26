#pragma once

#include "iw/renderer/ComputeShader.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLComputeShader
		: public IComputeShader
	{
	private:
		unsigned int m_computeShader;

	public:
		GLComputeShader(
			const char* source);

		~GLComputeShader();

		inline unsigned int ComputeShader() const {
			return m_computeShader;
		}
	};
}
}
