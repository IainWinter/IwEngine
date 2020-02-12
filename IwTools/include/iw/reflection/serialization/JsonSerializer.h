#pragma once

#include "Serializer.h"

namespace iw {
	// WIP
	class JsonSerializer
		: public Serializer
	{
	public:
		JsonSerializer()
			: Serializer()
		{}

		JsonSerializer(
			std::string filePath,
			bool overwrite = false)
			: Serializer(filePath, overwrite)
		{}

		void SerializeClass(
			const iw::Class* class_,
			const void* value) override
		{
			m_stream->put('{');

			Serializer::SerializeClass(class_, value);

			m_stream->seekp(-1, m_stream->cur);
			m_stream->put('}');
		}

		void SerializeField(
			const iw::Field& field,
			const void* value) override
		{
			m_stream->put('"');
			m_stream->write(field.name, strlen(field.name));
			m_stream->write("\": ", 3);

			SerializeType(field.type, value);

			m_stream->put(',');
		}

		virtual void SerializeFieldArray(
			const iw::Field& field,
			const iw::Type* elemType,
			const void* value,
			size_t size) override
		{
			m_stream->put('"');
			m_stream->write(field.name, strlen(field.name));
			m_stream->write("\": ", 3);

			SerializeArray(elemType, value, size);

			m_stream->put(',');
		}

		void SerializeArray(
			const iw::Type* type,
			const void* value,
			size_t count) override
		{
			if (type->type == CHAR) {
				m_stream->put('"');
				Write((char*)value, count);
				m_stream->put('"');
			}

			else {
				m_stream->put('[');

				for (int i = 0; i < count; i++) {
					void* ptr = (char*)value + i * type->size;
					SerializeType(type, ptr);

					if (i != count - 1) {
						m_stream->put(',');
					}
				}

				m_stream->put(']');
			}
		}

		void SerializeValue(
			const iw::Type* type,
			const void* value)
		{
			std::string str;
			switch (type->type) {
				case BOOL:               str = std::to_string(*(bool*)              value); break;
				case CHAR:               str = std::to_string(*(char*)              value); break;
				case SHORT:              str = std::to_string(*(short*)             value); break;
				case INT:                str = std::to_string(*(int*)               value); break;
				case LONG:               str = std::to_string(*(long*)              value); break;
				case FLOAT:              str = std::to_string(*(float*)             value); break;
				case DOUBLE:             str = std::to_string(*(double*)            value); break;
				case LONG_LONG:          str = std::to_string(*(long long*)         value); break;
				case LONG_DOUBLE:        str = std::to_string(*(long double*)       value); break;
				case UNSIGNED_CHAR:      str = std::to_string(*(unsigned char*)     value); break;
				case UNSIGNED_SHORT:     str = std::to_string(*(unsigned short*)    value); break;
				case UNSIGNED_INT:       str = std::to_string(*(unsigned int*)      value); break;
				case UNSIGNED_LONG:      str = std::to_string(*(unsigned long*)     value); break;
				case UNSIGNED_LONG_LONG: str = std::to_string(*(unsigned long long*)value); break;
			}

			Write(str.c_str(), str.length());
		}
		
	};
}
