#pragma once

#include "iw/util/iwutil.h"
#include "iw/reflection/Type.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>

namespace iw {
	class Serializer {
	protected:
		std::iostream* m_stream;

	public:
		IWUTIL_API
		Serializer();

		IWUTIL_API
		Serializer(
			std::string filePath,
			bool overwrite = false);

		IWUTIL_API
		~Serializer();

		IWUTIL_API
		void Flush();

		IWUTIL_API
		void Close();

		IWUTIL_API
		std::string Str() const;

		IWUTIL_API
		void ResetCursor();

		template<
			typename _t>
		void Write(
			const _t& value)
		{
			const iw::Type* type = GetType<_t>();
			SerializeType(type, &value);
		}

		template<
			typename _t>
		void Read(
			_t& value)
		{
			const iw::Type* type = GetType<_t>();
			DeserializeType(type, &value);
		}
	protected:
		IWUTIL_API
		virtual void SerializeClass(
			const iw::Class* class_,
			const void* value);

		IWUTIL_API
		virtual void SerializeField(
			const iw::Field& field,
			const void* value);

		IWUTIL_API
		virtual void SerializeType(
			const iw::Type* type,
			const void* value);

		IWUTIL_API
		virtual void SerializeValue(
			const iw::Type* type,
			const void* value);

		IWUTIL_API
		virtual void DeserializeClass(
			const iw::Class* class_,
			void* value);

		IWUTIL_API
		virtual void DeserializeField(
			const iw::Field& field,
			void* value);

		IWUTIL_API
		virtual void DeserializeType(
			const iw::Type* type,
			void* value);

		IWUTIL_API
		virtual void DeserializeValue(
			const iw::Type* type,
			void* value);

		IWUTIL_API
		void SerializeVector(
			const iw::Class* class_,
			const void* value);

		IWUTIL_API
		void SerializeString(
			const iw::Class* class_,
			const void* value);

		IWUTIL_API
		void DeserializeVector(
			const iw::Class* class_,
			void* value);

		IWUTIL_API
		void DeserializeString(
			const iw::Class* class_,
			void* value);

		IWUTIL_API
		void WriteToStream(
			const void* value,
			size_t size);

		IWUTIL_API
		void ReadFromStream(
			void* value,
			size_t size);
	};
}

// {
// "pos": {
//   "x" 1,
//   "y": 2
// }
// "arr": [1, 2, 3, 4]
//
//
//
//
// }
//
//

