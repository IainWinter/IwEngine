#pragma once

#include "iw/renderer/VertexShader.h"

namespace IwRenderer {
	class IWRENDERER_API GLVertexShader
		: public IVertexShader
	{
	private:
		unsigned int m_vertexShader;

	public:
		GLVertexShader(
			const char* source);

		~GLVertexShader();

		inline unsigned int VertexShader() const {
			return m_vertexShader;
		}
	};
}
