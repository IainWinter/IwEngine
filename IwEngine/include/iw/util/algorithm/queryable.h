#pragma once

#include "iw/util/iwutil.h"
#include <functional>
#include <vector>

namespace iwutil {
	template<
		typename _container>
	class queryable {
	public:
		using value_type     = typename _container::value_type;
		using iterator       = typename _container::iterator;
		using const_iterator = typename _container::const_iterator;

		using query_function = bool(const value_type&);

	private:
		_container m_data;
	public:
		queryable(
			const_iterator begin,
			const_iterator end)
			: m_data(begin, end)
		{}

		queryable where(
			std::function<query_function> query)
		{
			_container result;
			for (const value_type& item : m_data) {
				if (query(item)) {
					result.push_back(item);
				}
			}

			return queryable(result.begin(), result.end());
		}
	};
}
