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
		GLenum err = glGetError();
		bool hasError = err != GL_NO_ERROR;
		if (hasError) {
			LOG_ERROR << "GL ERROR: " << GetErrorString(err) << " in file " << fname << "[" << line << "] " << stmt;
		}

		return hasError;
	}
}

	using namespace RenderAPI;
}

#ifdef IW_DEBUG
#	define GL (stmt) stmt; while(iw::MessageCallback(#stmt, __FILE__, __LINE__)) {}
#	define GLe(stmt) stmt; while(iw::MessageCallback(#stmt, __FILE__, __LINE__)) { err = true; }
#else
#	define GL(stmt)  stmt
#	define GLe(stmt) stmt
#endif
