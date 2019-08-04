#pragma once

#include "iw/util/iwutil.h"

namespace iwutil {
	// Shared pointer for values not yet initialized
	template<
		typename _t>
	class potential {
		_t*   m_value;
		bool* m_initialized;
		int*   m_refcount; // not thread safe

	public:
		potential()
			: m_value(new _t[1])
			, m_initialized(new bool[1]{ false })
			, m_refcount(new int[1]{ 1 })
		{}

		potential(
			_t* value)
			: m_value(value)
			, m_initialized(new bool[1]{ true })
			, m_refcount(new int[1]{ 1 })
		{}

		potential(potential&& copy) noexcept
			: m_value(copy.m_value)
			, m_initialized(copy.m_initialized)
			, m_refcount(copy.m_refcount)
		{
			copy.m_value       = nullptr;
			copy.m_initialized = nullptr;
			copy.m_refcount    = nullptr;
		}

		potential(potential& copy)
			: m_value(copy.m_value)
			, m_initialized(copy.m_initialized)
			, m_refcount(copy.m_refcount)
		{
			++*m_refcount;
		}

		~potential() {
			if (m_refcount) {
				if (*m_refcount == 1) {
					delete[] m_value;
					delete[] m_initialized;
					delete[] m_refcount;
				}

				--*m_refcount;
			}
		}

		potential& operator=(
			potential&& copy) noexcept
		{
			m_value       = copy.m_value;
			m_initialized = copy.m_initialized;
			m_refcount    = copy.m_refcount;

			copy.m_value       = nullptr;
			copy.m_initialized = nullptr;
			copy.m_refcount    = nullptr;

			return *this;
		}

		potential& operator=(
			potential& copy)
		{
			m_value       = copy.m_value;
			m_initialized = copy.m_initialized;
			m_refcount    = copy.m_refcount;

			++*m_refcount;
			 
			return *this;
		}

		inline void initialize(
			const _t& value)
		{
			*m_value      = value;
			*m_initialized = true;
		}

		inline _t& value() {
			return *m_value;
		}

		inline const _t& value() const {
			return *m_value;
		}

		inline _t* ptr() {
			return m_value;
		}

		inline const _t* ptr() const {
			return m_value;
		}

		inline bool initialized() const {
			return *m_initialized;
		}
	};
}
