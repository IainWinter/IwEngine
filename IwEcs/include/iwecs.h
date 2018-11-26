#pragma once

#ifdef IWECS_EXPORT
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

#include <cstddef>

namespace iwecs {
	using entity_t = std::size_t;

	struct entity_data {
		const void* pointers;
		std::size_t pointers_size;
	};
}