#pragma once

#include <cstddef>
#include "input_iterator.h"

namespace iwutil {
	template<
		typename _t>
	class itight_container {
	public:
		using iterator = iwutil::input_iterator<_t>;
		using const_iterator = iwutil::input_iterator<const _t>;

	public:
		virtual ~itight_container() {}

		virtual std::size_t push_back(_t&& val)      = 0;
		virtual std::size_t push_back(const _t& val) = 0;
		virtual void erase(std::size_t index)        = 0;
		virtual bool contains(_t&& val)              = 0;
		virtual bool contains(const _t& val)         = 0;
		virtual _t& at(std::size_t index)            = 0;
		virtual _t* data()                           = 0;
		virtual iterator begin()                     = 0;
		virtual iterator end()                       = 0;
	};

	template<
		typename _t>
	class tight_container : public itight_container<_t> {
	protected:
		std::size_t m_size;

	public:
		tight_container()
			: m_size(0) {}

		virtual ~tight_container() {}

		//Returns if val is an element in the array
		bool contains(_t&& val) {
			for (auto itr = begin(); itr != end(); itr++) {
				if (*itr == std::forward<_t>(val)) {
					return true;
				}
			}

			return false;
		}

		//Returns if val is an element in the array
		bool contains(const _t& val) {
			for (auto itr = begin(); itr != end(); itr++) {
				if (*itr == val) {
					return true;
				}
			}

			return false;
		}

		_t& at(
			std::size_t index)
		{
			return data()[index];
		}

		iterator begin() {
			return iterator(data());
		}

		iterator end() {
			return iterator(data() + m_size);
		}
	};
}