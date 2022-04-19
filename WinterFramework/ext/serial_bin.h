#pragma once

#include "../Serial.h"

struct bin_writer : meta::serial_writer
{ 
	bin_writer(std::ostream& out)
		: meta::serial_writer(out, true)
	{}

	void write_type(meta::type* type, void* instance) override
	{
		auto& members = type->get_members();

		if (members.size() > 0)
		{
			for (int i = 0; i < members.size(); i++)
			{
				meta::type* member = members.at(i);
				write_type(member, member->walk_ptr(instance));
			}
		}

		else
		{
			type->serial_write(this, instance);
		}
	}

	void write_length(size_t length) override
	{
		serial_writer::write_value(length);
	}

	void write_array(meta::type* type, void* instance, size_t repeat) override
	{
		for (int i =  0; i < repeat; i++)
		{
			write_type(type, (char*)instance + i * type->m_info->m_size);
		}
	}

	void write_string(const char* str, size_t length) override
	{
		m_out.write(str, sizeof(char) * length);
	}
};

struct bin_reader : meta::serial_reader
{ 
	bin_reader(std::istream& in)
		: meta::serial_reader (in, true)
	{}

	void read_type(meta::type* type, void* instance) override
	{
		auto& members = type->get_members();

		if (members.size() > 0)
		{
			for (int i = 0; i < members.size(); i++)
			{
				meta::type* member = members.at(i);
				read_type(member, member->walk_ptr(instance));
			}
		}

		else
		{
			type->serial_read(this, instance);
		}
	}

	size_t read_length() override
	{
		size_t length = 0;
		read_value<size_t>(length);
		return length;
	}

	void read_array(meta::type* type, void* instance, size_t repeat) override
	{
		for (int i =  0; i < repeat; i++)
		{
			read_type(type, (char*)instance + i * type->m_info->m_size);
		}
	}

	void read_string(char* str, size_t length) override
	{
		m_in.read(str, sizeof(char) * length);
	}
};