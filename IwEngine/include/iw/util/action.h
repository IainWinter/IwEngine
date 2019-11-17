#pragma once

#include "iw/util/iwutil.h"

namespace iw {
inline namespace util {
	class iaction {
	public:
		virtual ~iaction() {}

		virtual void take() = 0;
	};

	template<
		typename...>
	class action;

	template<
		typename _arg_t>
	class action<_arg_t>
		: public iaction
	{
	private:
		using arg_type          = _arg_t;
		using function_type     = void(*)(arg_type);
		using arg_function_type = std::remove_reference_t<arg_type>(*)();

		function_type     m_action;
		arg_function_type m_args;

	public:
		action(
			function_type action,
			arg_function_type args)
			: m_action(action)
			, m_args(args)
		{}

		void take() override {
			m_action(m_args());
		}
	};

	template<
		typename _member_t,
		typename _arg_t>
	class action<_member_t, _arg_t>
		: public iaction
	{
	private:
		using member_type       = _member_t;
		using arg_type          = _arg_t;
		using function_type     = void(*)(arg_type);
		using arg_function_type = std::remove_reference_t<arg_type>(member_type::*)();

		function_type     m_action;
		arg_function_type m_args;

	public:
		action(
			function_type action,
			arg_function_type args)
			: m_action(action)
			, m_args(args)
		{}

		void take() override {
			//m_action(m_args());
		}

		void take(member_type& member) {
			m_action(member.m_args());
		}
	};
}
}
