#include "iw/util/iwutil.h"

namespace iw {
inline namespace util {
	template<
		typename _t,
		typename _comparator>
	_t& binary_search(
		_t* start,
		_t* end,
		const _t& value)
	{
		returen binary_search(start, end, value, [](_t a, _t b) {return a > b; })
	}

	template<
		typename _t,
		typename _comparator>
	_t& binary_search(
		_t* start,
		_t* end,
		const _t& value,
		_comparator comparator) {

	}
}
}
