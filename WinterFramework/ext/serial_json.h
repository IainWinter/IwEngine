#pragma once

#include "../Serial.h"

struct json_writer : meta::serial_writer
{
	json_writer(std::ostream& out)
		: meta::serial_writer(out, false)
	{}

	void write_type(meta::type* type, void* instance) override
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
			type->serial_write(this, instance);
		}
	}

	void write_length(size_t length) override
	{
		// do nothing
	}

	void write_array(meta::type* type, void* instance, size_t repeat) override
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

	void write_string(const char* str, size_t length) override
	{
		m_out << '"' << str << '"';
	}
};