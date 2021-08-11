#pragma once

#include "Serializer.h"
#include "json.h"
#include <stack>
#include <vector>
#include <assert.h>

namespace iw {
namespace Reflect {
	// WIP
	class JsonSerializer
		: public Serializer
	{
	private:
		json_value_s* m_json;
		std::stack<json_value_s*> m_current;

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
			if (source == "") {
				return; // No json to parse
			}

			m_json = json_parse(source.c_str(), source.size());
			m_current.push(m_json);

			// object
			//  name | object
			//   string | number
			//   name | array
			//    object
			//     name | number
			//      ...
			//      ...
			//   name | object
		}

		~JsonSerializer() {
			free(m_json);
		}

		void ResetCursor() override {
			m_current = {};
			m_current.push(m_json);
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
			const void* value) override
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
			void* value) override
		{
			assert(m_current.top()->type == json_type_object);

			json_object_element_s* field = json_value_as_object(m_current.top())->start;
			m_current.pop();

			// push in reverse order
			std::vector<json_value_s*> fields;
			while (field) {
				fields.push_back(field->value);
				field = field->next;
			}

			for (int i = 0; i < fields.size(); i++) {
				m_current.push(fields[fields.size() - i - 1]);
			}

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

		void DeserializeArray(
			const iw::Type* type,
			void* value,
			size_t count) override
		{
			if (type->type == IntegralType::CHAR) {
				assert(m_current.top()->type == json_type_string);
				DeserializeValue(type, value); // array of chars is a string
			}

			else {
				assert(m_current.top()->type == json_type_array);

				json_array_element_s* element = json_value_as_array(m_current.top())->start;
				m_current.pop();

				// push in reverse order
				std::vector<json_value_s*> elements;
				while (element) {
					elements.push_back(element->value);
					element = element->next;
				}

				for (int i = 0; i < elements.size(); i++) {
					m_current.push(elements[elements.size() - i - 1]);
				}

				for (int i = 0; i < count; i++) {
					void* ptr = (char*)value + i * type->size;
					DeserializeType(type, ptr);
				}
			}
		}

		void DeserializeValue(
			const iw::Type* type,
			void* value) override
		{
			int i = m_current.top()->type;
			switch (m_current.top()->type) {
				case json_type_number: {
					json_number_s* number = json_value_as_number(m_current.top());

					std::string str(number->number);
					switch (type->type) {
						case IntegralType::BOOL:               *(bool*)              value = str[0] == '1';         break;
						case IntegralType::CHAR:               *(char*)              value = str[0];                break;
						case IntegralType::SHORT:              *(short*)             value = std::stol(str);        break;
						case IntegralType::INT:                *(int*)               value = std::stoi(str);        break;
						case IntegralType::LONG:               *(long*)              value = std::stol(str);        break;
						case IntegralType::FLOAT:              *(float*)             value = std::stof(str);        break;
						case IntegralType::DOUBLE:             *(double*)            value = std::stod(str);        break;
						case IntegralType::LONG_LONG:          *(long long*)         value = std::stoll(str);       break;
						case IntegralType::LONG_DOUBLE:        *(long double*)       value = std::stold(str);       break;
						case IntegralType::UNSIGNED_CHAR:      *(unsigned char*)     value = (unsigned char)str[0]; break;
						case IntegralType::UNSIGNED_SHORT:     *(unsigned short*)    value = std::stoul(str);       break;
						case IntegralType::UNSIGNED_INT:       *(unsigned int*)      value = std::stoul(str);       break;
						case IntegralType::UNSIGNED_LONG:      *(unsigned long*)     value = std::stoul(str);       break;
						case IntegralType::UNSIGNED_LONG_LONG: *(unsigned long long*)value = std::stoull(str);      break;
					}
					break;
				}
				case json_type_string: {
					json_string_s* string = json_value_as_string(m_current.top());
					memcpy(value, string->string, string->string_size);
					break;
				}
				case json_type_true: {
					*(bool*)value = true;
					break;
				}
				case json_type_false: {
					*(bool*)value = false;
					break;
				}
				case json_type_null: {
					*(size_t*)value = (size_t)nullptr;
					break;
				}
				default: {
					assert("Invalid value in json");
				}
			}

			m_current.pop();
		}
	};
}

	using namespace Reflect;
}
