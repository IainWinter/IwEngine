#pragma once

// Supports all integral types (int, float, char...) and their respected fixed arrays
// Supports all classes generated from IwReflection.exe and their respected fixed arrays
// Doesn't support pointers

#include "Type.h"
#include <assert.h>
#include <cstddef>
#include <sstream>

#if !defined(__clang__) && !defined(__GNUC__)
	#ifdef __attribute__ 
		# undef __attribute__ 
	#endif
	#define __attribute__(a)
#else
	#define REFLECTING
#endif

#ifndef REFLECT
#	define REFLECT __attribute__((annotate("reflect")))
#endif
#ifndef NO_REFLECT
#	define NO_REFLECT __attribute__((annotate("no_reflect")))
#endif
#ifndef PASS_REFLECT
#	define PASS_REFLECT __attribute__((annotate("pass_reflect")))
#endif

#define IW_BEGIN_REFLECT namespace iw { namespace Reflect { namespace detail {
#define IW_END_REFLECT }}}

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
		static bool enumerated = false; // blocks infinite recursion
		static const Type* type;

		if constexpr (!std::is_arithmetic_v<_t>) {
			if (!enumerated) {
				enumerated = true;
				type = GetClass(ClassTag<_t>());
			}

			return type;
		}

		assert("No reflection information for type");
		throw nullptr;
	}

	inline const char* get_array_type_name(const char* type_name, size_t _s)
	{
		std::stringstream ss;
		ss << _s;

		size_t digits = ss.str().size();

		ss = std::stringstream();
		ss << "[";
		ss << _s;
		ss << "]\0";

		size_t size = strlen(type_name);
		size_t size_with_array = size + 5u + (digits - 1u);

		char* type_name_with_array = new char[size_with_array];

		memcpy_s(type_name_with_array, size_with_array, type_name, size);
		memcpy_s(type_name_with_array + size, size_with_array, ss.str().c_str(), 5u);

		return type_name_with_array;
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
