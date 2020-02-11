#pragma once

#include "Type.h"

#if !defined(__clang__) && !defined(__GNUC__)
	#ifdef __attribute__ 
		# undef __attribute__ 
	#endif
	#define __attribute__(a)
	#define REFLECTING 
#endif

#define REFLECT __attribute__((annotate("reflect")))

namespace iw {
namespace detail {
	template<
		typename _c>
	struct ClassTag {};

	template<
		typename _c,
		size_t _s>
	struct ClassTag<_c[_s]> {};

	template<
		typename _t>
	struct TypeTag {};

	template<
		typename _t,
		size_t _s>
	struct TypeTag<_t[_s]> {};

	template<
		typename _c>
	const Class* GetClass(
		ClassTag<_c>)
	{
		assert("No reflection information for type", typeid(_c).name());
		throw nullptr;
	}

	template<
		typename _t>
	const Type* GetType(
		TypeTag<_t>)
	{
		if constexpr (!std::is_arithmetic_v<_t>) {
			return GetClass(ClassTag<_t>());
		}

		assert("No reflection information for type", typeid(_t).name());
		throw nullptr;
	}
}

	template<
		typename _c>
	const Class* GetClass() {
		return iw::detail::GetClass(iw::detail::template ClassTag<_c>());
	}

	template<
		typename _t>
	const Type* GetType() {
		return iw::detail::GetType(iw::detail::template TypeTag<_t>());
	}

#define INTEGRAL(t, it, n, s)                                        \
	namespace detail {                                              \
		template<>                                                 \
		inline const Type* GetType(                                \
			TypeTag<t>)                                           \
		{                                                          \
			static Type type = { n, s, it };                      \
			return &type;                                         \
		}                                                          \
                                                                     \
		template<size_t _s>                                        \
		inline const Type* GetType(                                \
			TypeTag<t[_s]>)                                       \
		{                                                          \
			static Type type = { n"[]", s, it, false, true, _s }; \
			return &type;                                         \
		}                                                          \
	}                                                               \

	INTEGRAL(bool,               BOOL,               "bool",               sizeof(bool))
	INTEGRAL(char,               CHAR,               "char",               sizeof(char))
	INTEGRAL(short,              SHORT,              "short",              sizeof(short))
	INTEGRAL(int,                INT,                "int",                sizeof(int))
	INTEGRAL(long,               LONG,               "long",               sizeof(long))
	INTEGRAL(float,              FLOAT,              "float",              sizeof(float))
	INTEGRAL(double,             DOUBLE,             "double",             sizeof(double))
	INTEGRAL(long long,          LONG_LONG,          "long long",          sizeof(long long))
	INTEGRAL(long double,        LONG_DOUBLE,        "long double",        sizeof(long double))
	INTEGRAL(unsigned char,      UNSIGNED_CHAR,      "unsigned char",      sizeof(unsigned char))
	INTEGRAL(unsigned short,     UNSIGNED_SHORT,     "unsigned short",     sizeof(unsigned short))
	INTEGRAL(unsigned int,       UNSIGNED_INT,       "unsigned int",       sizeof(unsigned int))
	INTEGRAL(unsigned long,      UNSIGNED_LONG,      "unsigned long",      sizeof(unsigned long))
	INTEGRAL(unsigned long long, UNSIGNED_LONG_LONG, "unsigned long long", sizeof(unsigned long long))

#undef INTEGRAL
}
