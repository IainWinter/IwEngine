#pragma once

#include "iw/renderer/GeometryShader.h"

namespace IW {
namespace RenderAPI {
	class GLGeometryShader
		: public IGeometryShader
	{
	private:
		unsigned gl_id;

	public:
		IWRENDERER_API
		GLGeometryShader(
			const char* source);

		IWRENDERER_API
		~GLGeometryShader() = default;

		IWRENDERER_API
		unsigned GeometryShader() const;
	};
}

	using namespace RenderAPI;
}
