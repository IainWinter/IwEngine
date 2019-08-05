#pragma once

#include "iw/util/iwutil.h"
#include <memory>

namespace iwutil {
	// Shared pointer for values not yet initialized
	template<
		typename _t>
	class potential {
		std::shared_ptr<_t>   m_value;
		std::shared_ptr<bool> m_initialized;

	public:
		potential()
			: m_value(std::make_shared<_t>(_t()))
			, m_initialized(std::make_shared<bool>(false))
		{}

		potential(
			_t* value)
			: m_value(std::make_shared(value))
			, m_initialized(std::make_shared(true))
		{}

		potential(potential&& copy) noexcept
			: m_value(copy.m_value)
			, m_initialized(copy.m_initialized)
		{
			copy.m_value.reset();
			copy.m_initialized.reset();
		}

		potential(potential& copy)
			: m_value(copy.m_value)
			, m_initialized(copy.m_initialized)
		{}

		~potential() {
			m_value.reset();
			m_initialized.reset();
		}

		potential& operator=(
			potential&& copy) noexcept
		{
			m_value.reset();
			m_initialized.reset();

			m_value       = copy.m_value;
			m_initialized = copy.m_initialized;

			copy.m_value.reset();
			copy.m_initialized.reset();

			return *this;
		}

		potential& operator=(
			potential& copy)
		{
			m_value       = copy.m_value;
			m_initialized = copy.m_initialized;
			 
			return *this;
		}

		void release() {
			m_value.reset();
			m_initialized.reset();
		}

		void initialize(
			const _t& value)
		{
			*m_value       = value;
			*m_initialized = true;
		}

		_t& value() {
			return *m_value.get();
		}

		const _t& value() const {
			return *m_value.get();
		}

		inline _t* ptr() {
			return m_value;
		}

		inline const _t* ptr() const {
			return m_value;
		}

		bool initialized() const {
			return *m_initialized;
		}

		int use_count() const {
			return m_value.use_count();
		}
	};
}
