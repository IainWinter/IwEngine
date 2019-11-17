#pragma once

#include "iw/util/iwutil.h"

namespace iw {
inline namespace util {
	struct pair_hash {
		template<typename T1, typename T2>
		size_t operator() (const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};
}
}
