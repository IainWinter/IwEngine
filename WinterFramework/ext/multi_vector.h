#pragma once

#include "../Serial.h"

// a vector that can hold a references to any number of types
// each item stores its respected meta::type

namespace meta
{
	struct multi_item
	{
		meta::type* m_type;
		void* m_instance;
	};

	struct multi_vector
	{
		std::vector<multi_item> m_items;

		template<typename _t>
		void push(const _t* item)
		{
			m_items.push_back(multi_item { get_class<_t>(), (void*)item });
		}
		
		void push(const _t* item)
		{
			m_items.push_back(multi_item { get_class<_t>(), (void*)item });
		}

		void erase(const void* item)
		{
			for (int i = 0; i < m_items.size(); i++)
			{
				if (m_items.at(i).m_instance == item)
				{
					m_items.erase(m_items.begin() + i);
				}
			}
		}
	};

	template<>
	void serial_write(tag<multi_item>, serial_writer* serial, const multi_item& value)
	{
		serial->write_type(value.m_type, value.m_instance);
	}

	template<>
	void serial_read(tag<multi_item>, serial_reader* serial, multi_item& value)
	{
		serial->read_type(value.m_type, value.m_instance);
	}

	template<>
	void serial_write(tag<multi_vector>, serial_writer* serial, const multi_vector& value)
	{
		serial->write(value.m_items);
	}

	template<>
	void serial_read(tag<multi_vector>, serial_reader* serial, multi_vector& value)
	{
		serial->read(value.m_items);
	}
}