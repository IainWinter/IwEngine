#pragma once

#include <vector>
#include "tight_container.h"

namespace iwutil {
	template<
		typename _t>
	class tight_vector : public tight_container<_t> {
	private:
		std::vector<_t> m_vector;

	public:
		tight_vector()
			: m_vector() {}

		tight_vector(
			tight_vector&& copy)
			: m_vector(std::move(copy.m_vector))
			, m_size(copy.m_size)
		{
			copy.m_vector.clear();
			copy.m_size = 0;
		}

		tight_vector(
			const tight_vector& copy)
			: m_vector(copy.m_vector)
			, m_size(copy.m_size) {}

		~tight_vector() = default;

		//Insert data at m_size in the array
		std::size_t push_back(
			_t&& val) override
		{
			m_vector.push_back(std::forward<_t>(val));
			return m_size++;
		}

		//Insert data at m_size in the array
		std::size_t push_back(
			const _t& val) override
		{
			m_vector.push_back(val);
			return m_size++;
		}

		//Replaces nth element with element at m_size - 1
		//	and decrements the size
		void erase(
			std::size_t index) override
		{
			if(m_size > 0) {
				if (index != m_size - 1) {
					m_vector[index] = std::forward<_t>(m_vector[m_size - 1]);
				}

				m_vector.erase(m_vector.begin() + m_size - 1);
				m_size--;
			}
		}

		_t* data() {
			return m_vector.data();
		}
	};
}