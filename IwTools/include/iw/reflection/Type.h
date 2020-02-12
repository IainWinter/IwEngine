#pragma once

// Supports all integral types (int, float, char...) and their respected fixed arrays
// Supports all classes generated from IwReflection.exe
// Doesn't support arrays of classes
// Doesn't support pointers

namespace iw {
namespace Reflect {
	struct Type;
	struct Class;
	struct Field;
	class Serializer;

	enum class IntegralType {
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
		size_t count;      // Count of elements in supposed array

		Type(
			const char* name,
			size_t size,
			IntegralType type,
			bool isClass = false,
			bool isArray = false,
			size_t count = 1)
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
	};

	struct Field {
		const char* name;
		const Type* type;
		size_t offset;
	};

	struct Class
		: Type
	{
		Field* fields;
		size_t fieldCount;
		void(* serialize)  (Serializer& serializer, const void* data);
		void(* deserialize)(Serializer& serializer,       void* data);

		Class(
			const char* name,
			size_t size,
			size_t fieldCount)
			: Type(name, size, IntegralType::NOT_INTEGRAL, true)
			, fields(new Field[fieldCount])
			, fieldCount(fieldCount)
			, serialize  (nullptr)
			, deserialize(nullptr)
		{}

		Class(
			const char* name,
			size_t size,
			size_t fieldCount,
			size_t arrayCount)
			: Type(name, size, IntegralType::NOT_INTEGRAL, true, true, arrayCount)
			, fields(new Field[fieldCount])
			, fieldCount(fieldCount)
			, serialize  (nullptr)
			, deserialize(nullptr)
		{}

	};
}

	using namespace Reflect;
}
