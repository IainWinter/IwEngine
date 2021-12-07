#include "iwutil.h"

#include <string>
#include <iomanip> // required for std::setw

namespace iw
{
namespace util
{
	template<
		typename _t>
	std::string to_string(
		const _t& t)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << t;
		return ss.str();
	}
}

	using namespace util;
}
