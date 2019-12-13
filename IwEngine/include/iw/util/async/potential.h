#pragma once

#include "iw/util/iwutil.h"
#include <memory>

namespace iw {
namespace util {
	// Shared pointer for values not yet initialized
	template<
		typename _t>
	class potential {
	private:
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

		//TODO: Maybe check init?
		_t consume() {
			_t t = m_ptr->value;
			release();
			return t;
		}

		bool initialized() const {
			return m_ptr->initialized;
		}

		int use_count() const {
			return m_ptr.use_count();
		}
	};
}

	using namespace util;
}
