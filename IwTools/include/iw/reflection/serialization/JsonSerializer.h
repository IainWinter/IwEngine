#pragma once

#include "Serializer.h"
#include "json.h"
#include <streambuf>

namespace iw {
namespace Reflect {
	// WIP
	class JsonSerializer
		: public Serializer
	{
	private:
		json_value_s* m_json;
		json_value_s* m_current;

	public:
		JsonSerializer()
			: Serializer()
			, m_json(nullptr)
		{}

		JsonSerializer(
			std::string filePath,
			bool overwrite = false)
			: Serializer(filePath, overwrite)
			, m_json(nullptr)
		{
			std::string source = (std::stringstream() << m_stream->rdbuf()).str();
			m_current = m_json = json_parse(source.c_str(), source.size());
		}

		~JsonSerializer() {
			free(m_json);
		}

		void ResetCursor() override {
			m_current = m_json;
			Serializer::ResetCursor();
		}

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

		void SerializeFieldArray(
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
			if (type->type == IntegralType::CHAR) {
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
				case IntegralType::BOOL:               str = std::to_string(*(bool*)              value); break;
				case IntegralType::CHAR:               str = std::to_string(*(char*)              value); break;
				case IntegralType::SHORT:              str = std::to_string(*(short*)             value); break;
				case IntegralType::INT:                str = std::to_string(*(int*)               value); break;
				case IntegralType::LONG:               str = std::to_string(*(long*)              value); break;
				case IntegralType::FLOAT:              str = std::to_string(*(float*)             value); break;
				case IntegralType::DOUBLE:             str = std::to_string(*(double*)            value); break;
				case IntegralType::LONG_LONG:          str = std::to_string(*(long long*)         value); break;
				case IntegralType::LONG_DOUBLE:        str = std::to_string(*(long double*)       value); break;
				case IntegralType::UNSIGNED_CHAR:      str = std::to_string(*(unsigned char*)     value); break;
				case IntegralType::UNSIGNED_SHORT:     str = std::to_string(*(unsigned short*)    value); break;
				case IntegralType::UNSIGNED_INT:       str = std::to_string(*(unsigned int*)      value); break;
				case IntegralType::UNSIGNED_LONG:      str = std::to_string(*(unsigned long*)     value); break;
				case IntegralType::UNSIGNED_LONG_LONG: str = std::to_string(*(unsigned long long*)value); break;
			}

			Write(str.c_str(), str.length());
		}

		void DeserializeClass(
			const iw::Class* class_,
			void* value)
		{
			if (class_->deserialize) {
				class_->deserialize(*this, value);
			}

			else {
				for (int i = 0; i < class_->fieldCount; i++) {
					const iw::Field& f = class_->fields[i];
					DeserializeField(f, (char*)value + f.offset);
				}
			}
		}

		void DeserializeField(
			const iw::Field& field,
			void* value)
		{
			DeserializeType(field.type, value);
		}

		void DeserializeFieldArray(
			const iw::Field& field,
			const iw::Type* elemType,
			void* value,
			size_t size)
		{
			DeserializeArray(elemType, value, size);
		}

		void DeserializeType(
			const iw::Type* type,
			void* value)
		{
			if (type->isArray) {
				DeserializeArray(type, value, type->count);
			}

			else {
				void* ptr = (char*)value;
				if (type->isClass) {
					const iw::Class* class_ = type->AsClass();
					DeserializeClass(class_, ptr);
				}

				else {
					DeserializeValue(type, ptr);
				}
			}
		}

		void DeserializeArray(
			const iw::Type* type,
			void* value,
			size_t count)
		{
			if (type->type == IntegralType::CHAR) {
				Read((char*)value, count);
				m_stream->get(); // remove null char
			}

			else {
				for (int i = 0; i < count; i++) {
					void* ptr = (char*)value + i * type->size;
					DeserializeType(type, ptr);
				}
			}
		}

		void DeserializeValue(
			const iw::Type* type,
			void* value)
		{
			Read((char*)value, type->size);
		}

		void DeserializeValue(
			const iw::Type* type,
			const void* value)
		{
			std::string str((char*)value);
			switch (type->type) {
				case IntegralType::BOOL:               *(bool*)              value = str != "false" && str[0] != '0'; break;
				case IntegralType::CHAR:               *(char*)              value = str[0];                          break;
				case IntegralType::SHORT:              *(short*)             value = std::stol(str);                  break;
				case IntegralType::INT:                *(int*)               value = std::stoi(str);                  break;
				case IntegralType::LONG:               *(long*)              value = std::stol(str);                  break;
				case IntegralType::FLOAT:              *(float*)             value = std::stof(str);                  break;
				case IntegralType::DOUBLE:             *(double*)            value = std::stod(str);                  break;
				case IntegralType::LONG_LONG:          *(long long*)         value = std::stoll(str);                 break;
				case IntegralType::LONG_DOUBLE:        *(long double*)       value = std::stold(str);                 break;
				case IntegralType::UNSIGNED_CHAR:      *(unsigned char*)     value = (unsigned char)str[0];           break;
				case IntegralType::UNSIGNED_SHORT:     *(unsigned short*)    value = std::stoul(str);                 break;
				case IntegralType::UNSIGNED_INT:       *(unsigned int*)      value = std::stoul(str);                 break;
				case IntegralType::UNSIGNED_LONG:      *(unsigned long*)     value = std::stoul(str);                 break;
				case IntegralType::UNSIGNED_LONG_LONG: *(unsigned long long*)value = std::stoull(str);                break;
			}
		}
	};
}

	using namespace Reflect;
}
