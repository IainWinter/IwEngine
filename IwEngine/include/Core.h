#pragma once

#ifdef IWENGINE_EXPORT
#	define IWENGINE_API __declspec(dllexport)
#else
#	define IWENGINE_API __declspec(dllimport)
#endif

namespace IwEngine {}