#pragma once

#ifdef IW_BUILD_DLL
#	define IWENGINE_API __declspec(dllexport)
#else
#	define IWENGINE_API __declspec(dllimport)
#endif

namespace IwEngine {}