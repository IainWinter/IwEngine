#pragma once

#include "iw/renderer/GeometryShader.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLGeometryShader
		: public IGeometryShader
	{
	private:
		unsigned int m_geometryShader;

	public:
		GLGeometryShader(
			const char* source);

		~GLGeometryShader();

		inline unsigned int GeometryShader() const {
			return m_geometryShader;
		}
	};
}
}
