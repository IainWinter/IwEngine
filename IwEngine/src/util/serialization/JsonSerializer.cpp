#include "iw/util/serialization/JsonSerializer.h"
#include <sstream>
#include <fstream>

namespace iw {
	IWUTIL_API
	JsonSerializer::JsonSerializer()
		: Serializer()
	{}

	IWUTIL_API
	JsonSerializer::JsonSerializer(
		std::string filePath)
		: Serializer(filePath)
	{}

	void JsonSerializer::SerializeClass(
		const iw::Class* class_, 
		const void* value)
	{
		m_stream->put('{');

		if (strcmp("std::vector", class_->name) == 0) {
			SerializeVector(class_, value);
		}

		else if (strcmp("std::string", class_->name) == 0) {
			SerializeString(class_, value);
		}

		else {
			for (int i = 0; i < class_->fieldCount; i++) {
				const iw::Field& f = class_->fields[i];
				SerializeField(f, (char*)value + f.offset);

				if (i != class_->fieldCount - 1) {
					m_stream->put(',');
				}
			}
		}

		m_stream->put('}');
	}

	void JsonSerializer::SerializeField(
		const iw::Field& field,
		const void* value)
	{
		m_stream->put('"');
		m_stream->write(field.name, strlen(field.name));
		m_stream->write("\": ", 3);

		Serializer::SerializeField(field, value);
	}

	void JsonSerializer::SerializeValue(
		const iw::Type* type, 
		const void* value)
	{
		if (type->isArray) {
			m_stream->put('[');
		}

		for (int i = 0; i < type->count; i++) {
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

			m_stream->write(str.c_str(), str.length());

			if (i != type->count - 1) {
				m_stream->put(',');
			}
		}

		if (type->isArray) {
			m_stream->put(']');
		}
	}
}
