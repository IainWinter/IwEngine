#pragma once

#ifdef IW_BUILD_DLL
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

namespace iwecs {}