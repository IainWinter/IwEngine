#include "iw/renderer/Platform/OpenGL/GLTranslator.h"

namespace IW {
namespace RenderAPI {
	Translator<GLint, GLenum>& Translator<GLint, GLenum>::Instance() {
		static GLTranslator translator;
		return translator;
	}

	GLenum GLTranslator::Translate(
		TextureFormatType textureFormatType)
	{
		switch (textureFormatType) {
			case UBYTE: return GL_UNSIGNED_BYTE;
			case FLOAT: return GL_FLOAT;
		}

		return GL_INVALID_VALUE;
	}

	GLint GLTranslator::Translate(
		TextureFormat textureFormat, 
		TextureFormatType textureFormatType)
	{
		switch (textureFormat) {
			case ALPHA: {
				switch (textureFormatType) {
					case UBYTE: return GL_RED;
					case FLOAT: return GL_R32F;
				}
			}

			case RG: {
				switch (textureFormatType) {
					case UBYTE: return GL_RG;
					case FLOAT: return GL_RG32F;
				}
			}

			case RGB: {
				switch (textureFormatType) {
					case UBYTE: return GL_RGB;
					case FLOAT: return GL_RGB32F;
				}
			}

			case RGBA: {
				switch (textureFormatType) {
					case UBYTE: return GL_RGBA;
					case FLOAT: return GL_RGBA32F;
				}
			}

			case DEPTH: {
				switch (textureFormatType) {
					case UBYTE: return GL_DEPTH_COMPONENT;
					case FLOAT: return GL_DEPTH_COMPONENT32F;
				}
			}

			case STENCIL: {
				return GL_STENCIL_COMPONENTS;
			}
		}

		return GL_INVALID_VALUE;
	}

	GLint GLTranslator::Translate(
		TextureWrap textureWrap)
	{
		switch (textureWrap) {
			case EDGE:        return GL_CLAMP_TO_EDGE;
			case BORDER:      return GL_CLAMP_TO_BORDER;
			case MIRROR:      return GL_MIRRORED_REPEAT;
			case REPEAT:      return GL_REPEAT;
			case MIRROR_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
		}

		return GLU_INVALID_VALUE;
	}
}
}
