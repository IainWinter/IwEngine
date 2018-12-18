#pragma once

#include <vector>

namespace iwutil {
	template<
		typename _t>
	class sparse_set {
	public:
	private:
		std::vector<size_t> m_index;
		std::vector<_t> m_data;

	public:
		sparse_set() = default;
		~sparse_set() = default;
		sparse_set(const sparse_set&) = default;
		sparse_set(sparse_set&&) = default;

		sparse_set& operator=(const sparse_set&) = default;
		sparse_set& operator=(sparse_set&&) = default;
		
		//insert
		//	
		//erase
		//at
		//contains
		//operator[]
		//begin
		//end

	private:

	};
}