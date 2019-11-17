#pragma once

#include "iw/util/iwutil.h"
#include <stdint.h>

namespace iw {
inline namespace util {
	template<typename>
	struct type_traits;

	template<>
	struct type_traits<uint8_t> {
		using difference_type = int8_t;
	};

	template<>
	struct type_traits<uint16_t> {
		using difference_type = int16_t;
	};

	template<>
	struct type_traits<uint32_t> {
		using difference_type = int32_t;
	};

	template<>
	struct type_traits<uint64_t> {
		using difference_type = int64_t;
	};

	template<>
	struct type_traits<int8_t> {
		using difference_type = int8_t;
	};

	template<>
	struct type_traits<int16_t> {
		using difference_type = int16_t;
	};

	template<>
	struct type_traits<int32_t> {
		using difference_type = int32_t;
	};

	template<>
	struct type_traits<int64_t> {
		using difference_type = int64_t;
	};
}
}
