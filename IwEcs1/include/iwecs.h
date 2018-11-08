#pragma once

#ifdef IWECS_EXPORT
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

namespace iwecs {}
