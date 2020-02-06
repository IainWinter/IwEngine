#pragma once

namespace iw {
	struct Type;
	struct Class;
	struct Field;

	struct Type {
		const char* name;
		const size_t size;
		const bool isClass;

		Type(
			const char* name,
			size_t size,
			bool isClass = false)
			: name(name)
			, size(size)
			, isClass(isClass)
		{}

		const Class* AsClass() const {
			return (const Class*)this;
		}
	};

	struct Field {
		const char* name;
		const Type* type;
		size_t offset;
	};

	//struct NamedField {
	//	const char* name;
	//	Type& type;
	//	size_t offset;
	//};

	//struct Param {
	//	const char* name;
	//	Type& type;
	//};

	//struct Function {
	//	const char* name;
	//	Field returnValue;
	//	Param* parameters;
	//	size_t parameterCount;
	//};

	struct Class
		: Type
	{
		Field* fields;
		size_t fieldCount;
		//Function* functions;
		//size_t functionCount;

		Class(
			const char* name,
			size_t size,
			size_t fieldCount)
			: Type(name, size, true)
			, fields(new Field[fieldCount])
			, fieldCount(fieldCount)
		{}
	};

namespace detail {
	template<
		typename _c>
	struct ClassTag {};

	template<
		typename _t>
	const Class* GetClass(
		ClassTag<_t>)
	{
		static_assert("No reflection information for class");
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
		static_assert("No reflection information for type");
	}

	template<
		typename _t>
	char* SerializeType() {
		static_assert("No serialization information for type");
	}

#define PRIMITIVE(t, n, s)             \
	template<>                        \
	inline const Type* GetType<t>() { \
		static Type type = { n, s }; \
		return &type;                \
	}                                 \

	PRIMITIVE(bool,               "bool",               sizeof(bool))
	PRIMITIVE(char,               "char",               sizeof(char))
	PRIMITIVE(short,              "short",              sizeof(short))
	PRIMITIVE(int,                "int",                sizeof(int))
	PRIMITIVE(long,               "long",               sizeof(long))
	PRIMITIVE(long long,          "long long",          sizeof(long long))
	PRIMITIVE(float,              "float",              sizeof(float))
	PRIMITIVE(double,             "double",             sizeof(double))
	PRIMITIVE(long double,        "long double",        sizeof(long double))
	PRIMITIVE(unsigned char,      "unsigned char",      sizeof(unsigned char))
	PRIMITIVE(unsigned short,     "unsigned short",     sizeof(unsigned short))
	PRIMITIVE(unsigned int,       "unsigned int",       sizeof(unsigned int))
	PRIMITIVE(unsigned long,      "unsigned long",      sizeof(unsigned long))
	PRIMITIVE(unsigned long long, "unsigned long long", sizeof(unsigned long long))

#undef PRIMITIVE

//#define PRIMITIVE(t, n, s)                                        \
//	template<>                                                   \
//	inline char* SerializeType<t>() {                            \
//		char* buf = 					                     \
//		static Type type = { n, s };                            \
//		return &type;                                           \
//	}                                                            \
//
//	PRIMITIVE(bool, "bool", sizeof(bool))
//	PRIMITIVE(char, "char", sizeof(char))
//	PRIMITIVE(short, "short", sizeof(short))
//	PRIMITIVE(int, "int", sizeof(int))
//	PRIMITIVE(long, "long", sizeof(long))
//	PRIMITIVE(long long, "long long", sizeof(long long))
//	PRIMITIVE(float, "float", sizeof(float))
//	PRIMITIVE(double, "double", sizeof(double))
//	PRIMITIVE(long double, "long double", sizeof(long double))
//	PRIMITIVE(unsigned char, "unsigned char", sizeof(unsigned char))
//	PRIMITIVE(unsigned short, "unsigned short", sizeof(unsigned short))
//	PRIMITIVE(unsigned int, "unsigned int", sizeof(unsigned int))
//	PRIMITIVE(unsigned long, "unsigned long", sizeof(unsigned long))
//	PRIMITIVE(unsigned long long, "unsigned long long", sizeof(unsigned long long))
//
//#undef PRIMITIVE

namespace detail {
	//template<
	//	typename _t>
	//const Class* GetClass(
	//	ClassTag<std::vector<_t>>)
	//{
	//	static Class c;
	//	return &c;
	//}

	//template<
	//	typename _k,
	//	typename _v>
	//const Class* GetClass(
	//	ClassTag<std::map<_k, _v>>)
	//{
	//	static Class c;
	//	return &c;
	//}
}
}
