#pragma once

#include "iw/util/iwutil.h"
#include <memory>

namespace iwutil {
	// Shared pointer for values not yet initialized
	template<
		typename _t>
	class potential {
		struct value_t {
			_t   value;
			bool initialized;

			value_t(
				_t v,
				bool i)
				: value(v)
				, initialized(i)
			{}
		};

		std::shared_ptr<value_t> m_ptr;

	public:
		potential()
			: m_ptr(std::make_shared<value_t>(_t(), false))
		{}

		potential(
			const _t& value)
			: m_ptr(std::make_shared<value_t>(value, true))
		{}

		potential(potential&& copy) noexcept
			: m_ptr(copy.m_ptr)
		{
			copy.m_ptr.reset();
		}

		potential(potential& copy)
			: m_ptr(copy.m_ptr)
		{}

		~potential() {
			m_ptr.reset();
		}

		potential& operator=(
			potential&& copy) noexcept
		{
			m_ptr.reset();
			m_ptr = copy.m_value;
			copy.m_ptr.reset();

			return *this;
		}

		potential& operator=(
			potential& copy)
		{
			m_ptr = copy.m_ptr;
			 
			return *this;
		}

		void release() {
			m_ptr.reset();
		}

		void initialize(
			const _t& value)
		{
			m_ptr->value       = value;
			m_ptr->initialized = true;
		}

		_t& value() {
			return m_ptr->value;
		}

		const _t& value() const {
			return m_ptr->value;
		}

		bool initialized() const {
			return m_ptr->initialized;
		}

		int use_count() const {
			return m_ptr.use_count();
		}
	};
}
