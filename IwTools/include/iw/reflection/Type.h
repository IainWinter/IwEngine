#pragma once

namespace iw {
	struct Type;
	struct Class;
	struct Field;

	enum IntegralType {
		NOT_INTEGRAL,
		BOOL,
		CHAR,
		SHORT,
		INT,
		LONG,
		FLOAT,
		DOUBLE,
		LONG_LONG,
		LONG_DOUBLE,
		UNSIGNED_CHAR,
		UNSIGNED_SHORT,
		UNSIGNED_INT,
		UNSIGNED_LONG,
		UNSIGNED_LONG_LONG
	};

	struct Type {
		const char* name;
		const size_t size;
		const IntegralType type;
		const bool isClass;

		Type(
			const char* name,
			size_t size,
			IntegralType type,
			bool isClass = false)
			: name(name)
			, size(size)
			, type(type)
			, isClass(isClass)
		{}

		const Class* AsClass() const {
			return (const Class*)this;
		}

		template<
			typename _s>
		void Serialize(
			_s& stream,
			const void* value) const
		{
			switch (type) {
				case BOOL:               stream << *(bool*)              value; break;
				case CHAR:               stream << *(char*)              value; break;
				case SHORT:              stream << *(short*)             value; break;
				case INT:                stream << *(int*)               value; break;
				case LONG:               stream << *(long*)              value; break;
				case FLOAT:              stream << *(float*)             value; break;
				case DOUBLE:             stream << *(double*)            value; break;
				case LONG_LONG:          stream << *(long long*)         value; break;
				case LONG_DOUBLE:        stream << *(long double*)       value; break;
				case UNSIGNED_CHAR:      stream << *(unsigned char*)     value; break;
				case UNSIGNED_SHORT:     stream << *(unsigned short*)    value; break;
				case UNSIGNED_INT:       stream << *(unsigned int*)      value; break;
				case UNSIGNED_LONG:      stream << *(unsigned long*)     value; break;
				case UNSIGNED_LONG_LONG: stream << *(unsigned long long*)value; break;
			}
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
			: Type(name, size, NOT_INTEGRAL, true)
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

#define INTEGRAL(t, it, n, s)              \
	template<>                            \
	inline const Type* GetType<t>() {     \
		static Type type = { n, s, it }; \
		return &type;                    \
	}                                     \

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

//#define PRIMITIVE(t, s)                           \
//	template<>                                   \
//	inline void SerializeType<t>(t v) {   \
//		static char* buf = new char[s + 1];     \
//		buf[s] = '\0';                          \
//		memcpy(buf, &v, s);                     \
//		return;                                 \
//	}                                            \
//
//	PRIMITIVE(bool,               sizeof(bool))
//	PRIMITIVE(char,               sizeof(char))
//	PRIMITIVE(short,              sizeof(short))
//	PRIMITIVE(int,                sizeof(int))
//	PRIMITIVE(long,               sizeof(long))
//	PRIMITIVE(long long,          sizeof(long long))
//	//PRIMITIVE(float,              sizeof(float))
//	PRIMITIVE(double,             sizeof(double))
//	PRIMITIVE(long double,        sizeof(long double))
//	PRIMITIVE(unsigned char,      sizeof(unsigned char))
//	PRIMITIVE(unsigned short,     sizeof(unsigned short))
//	PRIMITIVE(unsigned int,       sizeof(unsigned int))
//	PRIMITIVE(unsigned long,      sizeof(unsigned long))
//	PRIMITIVE(unsigned long long, sizeof(unsigned long long))
//
//#undef PRIMITIVE

//#define PRIMITIVE(t, n, s)                                      \
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
