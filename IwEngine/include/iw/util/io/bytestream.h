#pragma once

#include "iw/util/iwutil.h"
#include <sstream>

namespace iw {
namespace util {
	class bytestream
		: std::stringstream
	{
	public:
		template<
			typename _t>
		bytestream& operator<<(
			_t& value)
		{
			for (int b = 0; b < iw::GetType<_t>()->size; b++) {
				char* byte = (char*)&value + b;
				std::stringstream::put(*byte);
			}

			return *this;
		}

		template<
			typename _t>
		bytestream& operator>>(
			_t& value)
		{
			char* ptr = (char*)&value;
			char byte;
			for (int b = 0; b < iw::GetType<_t>()->size; b++) {
				std::stringstream::get(byte);
				ptr[b] = byte;
			}

			return *this;
		}

		std::string str() {
			return std::stringstream::str();
		}
	};
}

	using namespace util;
}
