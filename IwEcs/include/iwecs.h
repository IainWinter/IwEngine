#pragma once

#ifdef IWINPUT_EXPORT
#	define IWINPUT_API __declspec(dllexport)
#else
#	define IWINPUT_API __declspec(dllimport)
#endif

namespace iwecs {}
#define iwinput iwecs
