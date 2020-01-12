#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "iw/util/enum/val.h"

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

		return GL_INVALID_VALUE;
	}

	GLint GLTranslator::Translate(
		GLenum glenum,
		unsigned& count)
	{
		switch (glenum) {
			case GL_BOOL:              count = 1;  break;
			case GL_INT:               count = 1;  break;
			case GL_UNSIGNED_INT:      count = 1;  break;
			case GL_FLOAT:             count = 1;  break;
			case GL_DOUBLE:            count = 1;  break;
			case GL_BOOL_VEC2:		   count = 2;  break;
			case GL_INT_VEC2:		   count = 2;  break;
			case GL_UNSIGNED_INT_VEC2: count = 2;  break;
			case GL_DOUBLE_VEC2:	   count = 2;  break;
			case GL_FLOAT_VEC2:		   count = 2;  break;
			case GL_BOOL_VEC3:		   count = 3;  break;
			case GL_INT_VEC3:		   count = 3;  break;
			case GL_UNSIGNED_INT_VEC3: count = 3;  break;
			case GL_FLOAT_VEC3:		   count = 3;  break;
			case GL_DOUBLE_VEC3:	   count = 3;  break;
			case GL_BOOL_VEC4:		   count = 4;  break;
			case GL_INT_VEC4:		   count = 4;  break;
			case GL_UNSIGNED_INT_VEC4: count = 4;  break;
			case GL_FLOAT_VEC4:		   count = 4;  break;
			case GL_DOUBLE_VEC4:	   count = 4;  break;
			case GL_FLOAT_MAT2:        count = 4;  break;
			case GL_FLOAT_MAT2x3:      count = 6;  break;
			case GL_FLOAT_MAT2x4:      count = 8;  break;
			case GL_FLOAT_MAT3:        count = 9;  break;
			case GL_FLOAT_MAT3x2:      count = 6;  break;
			case GL_FLOAT_MAT3x4:      count = 12; break;
			case GL_FLOAT_MAT4:        count = 16; break;
			case GL_FLOAT_MAT4x2:      count = 8;  break;
			case GL_FLOAT_MAT4x3:      count = 12; break;
			case GL_DOUBLE_MAT2:       count = 4;  break;
			case GL_DOUBLE_MAT2x3:     count = 6;  break;
			case GL_DOUBLE_MAT2x4:     count = 8;  break;
			case GL_DOUBLE_MAT3:       count = 9;  break;
			case GL_DOUBLE_MAT3x2:     count = 6;  break;
			case GL_DOUBLE_MAT3x4:     count = 12; break;
			case GL_DOUBLE_MAT4:       count = 16; break;
			case GL_DOUBLE_MAT4x2:     count = 8;  break;
			case GL_DOUBLE_MAT4x3:     count = 12; break;
			case GL_SAMPLER_2D:        count = 0;  break;
		}

		switch (glenum) {
			case GL_BOOL_VEC2:
			case GL_BOOL_VEC3:
			case GL_BOOL_VEC4:
			case GL_BOOL:              return iw::val(UniformType::BOOL);
			case GL_INT_VEC2:
			case GL_INT_VEC3:
			case GL_INT_VEC4:
			case GL_INT:               return iw::val(UniformType::INT);
			case GL_UNSIGNED_INT_VEC2:
			case GL_UNSIGNED_INT_VEC3:
			case GL_UNSIGNED_INT_VEC4:
			case GL_UNSIGNED_INT:      return iw::val(UniformType::UINT);
			case GL_FLOAT_MAT2:
			case GL_FLOAT_MAT2x3:
			case GL_FLOAT_MAT2x4:
			case GL_FLOAT_MAT3:
			case GL_FLOAT_MAT3x2:
			case GL_FLOAT_MAT3x4:
			case GL_FLOAT_MAT4:
			case GL_FLOAT_MAT4x2:
			case GL_FLOAT_MAT4x3:
			case GL_FLOAT_VEC2:
			case GL_FLOAT_VEC3:
			case GL_FLOAT_VEC4:
			case GL_FLOAT:             return iw::val(UniformType::FLOAT);
			case GL_DOUBLE_MAT2:
			case GL_DOUBLE_MAT2x3:
			case GL_DOUBLE_MAT2x4:
			case GL_DOUBLE_MAT3:
			case GL_DOUBLE_MAT3x2:
			case GL_DOUBLE_MAT3x4:
			case GL_DOUBLE_MAT4:
			case GL_DOUBLE_MAT4x2:
			case GL_DOUBLE_MAT4x3:
			case GL_DOUBLE_VEC2:
			case GL_DOUBLE_VEC3:
			case GL_DOUBLE_VEC4:
			case GL_DOUBLE:            return iw::val(UniformType::DOUBLE);
			case GL_SAMPLER_2D:        return iw::val(UniformType::SAMPLE2);
		}

		return GL_INVALID_VALUE;
	}
}
}
