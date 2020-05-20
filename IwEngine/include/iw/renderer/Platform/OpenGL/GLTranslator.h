#pragma once

#include "iw/renderer/Translator.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	class GLTranslator
		: public Translator<GLint, GLenum>
	{
	public:
		IWRENDERER_API
		GLenum Translate(
			CullFace cull) override; 

		IWRENDERER_API
		GLint Translate(
			TextureType textureType) override;

		IWRENDERER_API
		GLenum Translate(
			TextureFormatType textureFormatType) override;

		IWRENDERER_API
		GLint Translate(
			TextureFormat textureFormat,
			TextureFormatType textureFormatType) override;

		IWRENDERER_API
		GLint Translate(
			TextureFormat textureFormat) override;

		IWRENDERER_API
		GLint Translate(
			TextureWrap textureWrap) override;

		IWRENDERER_API
		GLint Translate(
			TextureFilter textureFilter) override;

		IWRENDERER_API
		GLint Translate(
			TextureMipmapFilter textureMipmapFilter) override;

		// Uniform type conversion
		IWRENDERER_API
		GLint Translate(
			GLenum glenum,
			unsigned& count) override;

		IWRENDERER_API
		GLint Translate(
			BufferType bufferType) override;

		IWRENDERER_API
		GLint Translate(
			BufferIOType bufferIOType) override;
	};
}

	using namespace RenderAPI;
}
