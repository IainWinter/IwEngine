#pragma once

#include "iw/renderer/Translator.h"
#include "gl/glew.h"

namespace IW {
namespace RenderAPI {
	class GLTranslator
		: public Translator<GLint, GLenum>
	{
	public:
		GLenum Translate(
			TextureFormatType textureFormatType) override;

		GLint Translate(
			TextureFormat textureFormat,
			TextureFormatType textureFormatType) override;

		GLint Translate(
			TextureWrap textureWrap) override;

		GLint Translate(
			GLenum glenum,
			unsigned& count) override;
	};
}

	using namespace RenderAPI;
}
