#pragma once

#include "iw/log/logger.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	inline constexpr char const* GetErrorString(
		GLenum err) noexcept
	{
		switch (err) {
			case GL_NO_ERROR:                      return "GL_NO_ERROR";
			case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
			case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
			case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
			case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
			case GL_TABLE_TOO_LARGE:               return "GL_TABLE_TOO_LARGE";
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
			default:                               return "unknown error";
		}
	}

	inline bool MessageCallback(
		const char* stmt,
		const char* fname,
		int line)
	{
		int finite = 255;
		bool hasError = false;

		do {
			GLenum err = glGetError();
			hasError = err != GL_NO_ERROR;
			if (hasError) {
				const char* str = GetErrorString(err);
				LOG_ERROR << "GL ERROR: " << str << " in file " << fname << "[" << line << "] " << stmt;
			}
		} while (hasError && --finite > 0);

		return hasError;
	}
}

	using namespace RenderAPI;
}

#ifdef IW_DEBUG
#	define GL(stmt)  stmt; if (iw::MessageCallback(#stmt, __FILE__, __LINE__)) {}
#	define GLe(stmt) stmt; if (iw::MessageCallback(#stmt, __FILE__, __LINE__)) { err = true; }
#else
#	define GL(stmt)  stmt
#	define GLe(stmt) stmt
#endif
