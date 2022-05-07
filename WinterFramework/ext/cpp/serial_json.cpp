#include "../serial_json.h"

// writer

void json_writer::write_type(meta::type* type, void* instance)
{
	auto& members = type->get_members();

	if (members.size() > 0) // json objects
	{
		m_out << '{';
		for (int i = 0; i < members.size(); i++)
		{
			meta::type* member = members.at(i);
			m_out << '"' << member->member_name() << '\"' << ':';
			write_type(member, member->walk_ptr(instance));

			if (i != members.size() - 1)
			{
				m_out << ',';
			}
		}
		m_out << '}';
	}

	else // json values
	{
		type->_serial_write(this, instance);
	}
}

void json_writer::write_length(size_t length)
{
	// do nothing
}

void json_writer::write_array(meta::type* type, void* instance, size_t repeat)
{
	m_out << '[';
	for (int i =  0; i < repeat; i++)
	{
		write_type(type, (char*)instance + i * type->m_info->m_size);

		if (i != repeat - 1)
		{
			m_out << ',';
		}
	}
	m_out << ']';
}

void json_writer::write_string(const char* str, size_t length)
{
	m_out << '"' << str << '"';
}

// reader

#include "json/json.h"

void json_reader::init_json()
{
	std::stringstream ss; ss << m_in.rdbuf();
	std::string str = ss.str(); // does this do a big copy?

	m_json = json_parse(str.c_str(), str.size());
}

void json_reader::read_type(meta::type* type, void* instance)
{
	if (!is_valid())
	{
		printf("Error with json reader, invaild json\n");
		return;
	}

	switch (m_json->type)
	{
		case json_type_object: // recurse if complex
		{
			json_object_s* object = json_value_as_object(m_json);
			assert(object->length == type->get_members().size() && "json object length did not match reader length");

			auto current_type = type->get_members().begin();
			json_object_element_s* current_member = object->start;

			while (current_member)
			{
				m_json = current_member->value;

				meta::type* curtype = *current_type;
				read_type(curtype, curtype->walk_ptr(instance));

				current_member = current_member->next;
				++current_type;
			}

			break;
		}
		case json_type_string: // custom handler for strings and arrays
		case json_type_array:
		{
			type->_serial_read(this, instance);
			break;
		}
		case json_type_false: // read value directly for base types, skip read_value
		case json_type_true:
		{
			*(bool*)instance = m_json->payload;
			break;
		}
		case json_type_number:
		{
			json_number_s* number = json_value_as_number(m_json);

			if (type->m_info->m_is_floating)
			{
				// no switch bc types sizes could be equal

				size_t s = type->m_info->m_size;

				     if (s == sizeof(      float)) *(      float*)instance = std::stof (number->number);
				else if (s == sizeof(     double)) *(     double*)instance = std::stod (number->number);
				else if (s == sizeof(long double)) *(long double*)instance = std::stold(number->number);
				
				else
				{
					assert(false && "json floating value had a invalid size");
				}
			}

			else
			{
				unsigned long long largest = std::stoull(number->number);
				memcpy(instance, &largest, type->m_info->m_size);
			}

			break;
		}
		case json_type_null:
		default:
			break;
	}
}

size_t json_reader::read_length() // returns the length of a string or array
{
	if (m_json->type == json_type_array)
	{
		return json_value_as_array(m_json)->length;
	}

	if (m_json->type == json_type_string)
	{
		return json_value_as_string(m_json)->string_size;
	}

	assert(false && "json was not an array or string");
	return 0;
}

void json_reader::read_array(meta::type* type, void* instance, size_t repeat) // puts each element into instance
{
	assert(m_json->type == json_type_array && "json type was not array");
	json_array_s* array = json_value_as_array(m_json);
	assert(array->length == repeat && "json array length did not match reader length");

	json_array_element_s* element = array->start;

	for (int i = 0 ; i < array->length; i++)
	{
		m_json = element->value;
		element = element->next;
		read_type(type, (char*)instance + i * type->m_info->m_size);
	}
}

void json_reader::read_string(char* str, size_t length) // puts each char into str
{
	assert(m_json->type == json_type_string && "json type was not string");
	json_string_s* string = json_value_as_string(m_json);
	assert(string->string_size == length && "json string length did not match reader length");
	memcpy(str, string->string, string->string_size);
}