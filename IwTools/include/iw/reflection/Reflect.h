#pragma once

// Supports all integral types (int, float, char...) and their respected fixed arrays
// Supports all classes generated from IwReflection.exe and their respected fixed arrays
// Doesn't support pointers

#include "Type.h"
#include <assert.h>
#include <cstddef>

#if !defined(__clang__) && !defined(__GNUC__)
	#ifdef __attribute__ 
		# undef __attribute__ 
	#endif
	#define __attribute__(a)
	#define REFLECTING 
#endif

#define REFLECT __attribute__((annotate("reflect")))


namespace iw {
namespace Reflect {
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
		assert("No reflection information for class");
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

		assert("No reflection information for type");
		throw nullptr;
	}
}

	template<
		typename _c>
	const Class* GetClass() {
		return detail::GetClass(detail::template ClassTag<_c>());
	}

	template<
		typename _t>
	const Type* GetType() {
		return detail::GetType(detail::template TypeTag<_t>());
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

	INTEGRAL(bool,               IntegralType::BOOL,               "bool",               sizeof(bool))
	INTEGRAL(char,               IntegralType::CHAR,               "char",               sizeof(char))
	INTEGRAL(short,              IntegralType::SHORT,              "short",              sizeof(short))
	INTEGRAL(int,                IntegralType::INT,                "int",                sizeof(int))
	INTEGRAL(long,               IntegralType::LONG,               "long",               sizeof(long))
	INTEGRAL(float,              IntegralType::FLOAT,              "float",              sizeof(float))
	INTEGRAL(double,             IntegralType::DOUBLE,             "double",             sizeof(double))
	INTEGRAL(long long,          IntegralType::LONG_LONG,          "long long",          sizeof(long long))
	INTEGRAL(long double,        IntegralType::LONG_DOUBLE,        "long double",        sizeof(long double))
	INTEGRAL(unsigned char,      IntegralType::UNSIGNED_CHAR,      "unsigned char",      sizeof(unsigned char))
	INTEGRAL(unsigned short,     IntegralType::UNSIGNED_SHORT,     "unsigned short",     sizeof(unsigned short))
	INTEGRAL(unsigned int,       IntegralType::UNSIGNED_INT,       "unsigned int",       sizeof(unsigned int))
	INTEGRAL(unsigned long,      IntegralType::UNSIGNED_LONG,      "unsigned long",      sizeof(unsigned long))
	INTEGRAL(unsigned long long, IntegralType::UNSIGNED_LONG_LONG, "unsigned long long", sizeof(unsigned long long))

#undef INTEGRAL
}

	using namespace Reflect;
}
