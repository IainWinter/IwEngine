#pragma once

// Supports all integral types (int, float, char...) and their respected fixed arrays
// Supports all classes generated from IwReflection.exe
// Doesn't support arrays of classes
// Doesn't support pointers

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
		const char* name;        // Literal type name 
		const size_t size;       // Size in bytes of type
		const IntegralType type; // Integral type (NOT_INTEGRAL) for classes
		const bool isClass;      // If AsClass() is valid
		const bool isArray;      // If type is an array
		const size_t count;      // Count of elements in supposed array

		Type(
			const char* name,
			size_t size,
			IntegralType type,
			bool isClass = false,
			bool isArray = false,
			size_t count = 0)
			: name(name)
			, size(size)
			, type(type)
			, isClass(isClass)
			, isArray(isArray)
			, count(count)
		{}

		const Class* AsClass() const {
			return isClass ? (const Class*)this : nullptr;
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

		template<
			typename _s>
		void Deserialize(
			_s& stream,
			void* value) const
		{
			switch (type) {
				case BOOL:               stream >> *(bool*)              value; break;
				case CHAR:               stream >> *(char*)              value; break;
				case SHORT:              stream >> *(short*)             value; break;
				case INT:                stream >> *(int*)               value; break;
				case LONG:               stream >> *(long*)              value; break;
				case FLOAT:              stream >> *(float*)             value; break;
				case DOUBLE:             stream >> *(double*)            value; break;
				case LONG_LONG:          stream >> *(long long*)         value; break;
				case LONG_DOUBLE:        stream >> *(long double*)       value; break;
				case UNSIGNED_CHAR:      stream >> *(unsigned char*)     value; break;
				case UNSIGNED_SHORT:     stream >> *(unsigned short*)    value; break;
				case UNSIGNED_INT:       stream >> *(unsigned int*)      value; break;
				case UNSIGNED_LONG:      stream >> *(unsigned long*)     value; break;
				case UNSIGNED_LONG_LONG: stream >> *(unsigned long long*)value; break;
			}
		}

		template<
			typename _t,
			typename _s>
		_t Deserialize(
			_s& stream) const
		{
			_t value;
			Deserialize(stream, &value);
			return value;
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

		template<
			typename _s>
		void Serialize(
			_s& stream,
			const void* value) const
		{
			for (int i = 0; i < fieldCount; i++) {
				iw::Field& field = fields[i];
				char* ptr = (char*)value + field.offset;

				if (field.type->isClass) {
					field.type->AsClass()->Serialize(stream, ptr);
				}

				else {
					if (field.type->isArray) {
						for (int e = 0; e < field.type->count; e++) {
							field.type->Serialize(stream, ptr + e * field.type->size);
						}
					}

					else {
						field.type->Serialize(stream, ptr);
					}
				}
			}
		}

		template<
			typename _s>
		void Deserialize(
			_s& stream,
			void* value) const
		{
			for (int i = 0; i < fieldCount; i++) {
				iw::Field& field = fields[i];
				char* ptr = (char*)value + field.offset;

				if (field.type->isClass) {
					field.type->AsClass()->Deserialize(stream, ptr);
				}

				else {
					if (field.type->isArray) {
						for (int e = 0; e < field.type->count; e++) {
							field.type->Deserialize(stream, ptr + e * field.type->size);
						}
					}

					else {
						field.type->Deserialize(stream, ptr);
					}
				}
			}
		}

		template<
			typename _t,
			typename _s>
		_t Deserialize(
			_s& stream) const
		{
			_t value;
			Deserialize(stream, &value);
			return value;
		}
	};

namespace detail {
	template<
		typename _c>
	struct ClassTag {};

	template<
		typename _t>
	struct TypeTag {};

	template<
		typename _t,
		size_t _s>
	struct TypeTag<_t[_s]> {};

	template<
		typename _t>
	const Class* GetClass(
		ClassTag<_t>)
	{
		static_assert("No reflection information for class");
		throw nullptr;
	}

	template<
		typename _t>
	const Class* GetType(
		TypeTag<_t>)
	{
		if constexpr (!std::is_integral_v<_t>) {
			return GetClass(ClassTag<_t>());
		}

		static_assert("No reflection information for type");
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

#define INTEGRAL(t, it, n, s)                                         \
	template<>                                                       \
	inline const Type* GetType<t>() {                                \
		static Type type = { n, s, it };                            \
		return &type;                                               \
	}                                                                \
	namespace detail {                                               \
		template<size_t _s>                                         \
		inline const Type* GetType(                                 \
			TypeTag<t[_s]>)                                        \
		{                                                           \
			static Type type = { n, s, it, false, true, _s }; \
			return &type;                                          \
		}                                                           \
	}                                                                \

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

	template<
		typename _t,
		typename _s>
	void Serialize(
		_s& stream,
		const _t& value)
	{
		const iw::Type* type = GetType<_t>();
		if (type->isClass) {
			type->AsClass()->Serialize(stream, &value);
		}

		else {
			type->Serialize(stream, &value);
		}
	}

	template<
		typename _t,
		typename _s>
	void Deserialize(
		_s& stream,
		_t& value)
	{
		const iw::Type* type = GetType<_t>();
		if (type->isClass) {
			type->AsClass()->Deserialize(stream, &value);
		}

		else {
			type->Deserialize(stream, &value);
		}
	}

	template<
		typename _t,
		typename _s>
	_t Deserialize(
		_s& stream)
	{
		_t value;
		Deserialize(stream, value);
		return value;
	}

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
