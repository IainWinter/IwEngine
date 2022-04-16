#pragma once

#include <ostream>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <string>
#include <functional>
#include <iomanip>
#include <string.h>

// ! This doesnt support pointers !
// Those are tricky and need a pointer table or something to be generated
// which is above what this is doing
// all components should use entity handles as pointers and std::containers for lists/ect

// specialize this as needed

template<typename _t>
struct meta_tag {};

template<typename _t>
std::string meta_typename(meta_tag<_t>)
{
	return typeid(_t).name();
}

template <typename _m>
struct _ptrtype // https://stackoverflow.com/a/22851790
{
   template <typename _t, typename _internal>
   static _internal get_type(_internal _t::*);
   typedef decltype(get_type(static_cast<_m>(nullptr))) type;
};
template<auto _m>
using ptrtype = typename _ptrtype<decltype(_m)>::type;

struct meta_type_info
{
	std::string m_name;
	size_t m_size;

	//bool m_is_pod;         // could serialize pod as plain bytes, ie anything without containers
	//bool m_is_string; // strings are special because they are arrays, but have quotes

	//bool m_is_array; // for array types
	//size_t m_array_size;

	//std::function<void(serial_writer*, void*)> m_encode;
};

struct serial_writer;
struct serial_reader;

struct meta_type
{
	meta_type_info* m_info;
	
	meta_type(
		meta_type_info* info
	)
		: m_info (info)
	{}

	virtual bool is_member() const = 0;

	// asserts m_has_members = true
	virtual const std::vector<meta_type*>& get_members() const = 0;

	// asserts m_is_member = true
	virtual const std::string& member_name() const = 0;

	// walks a pointer forward to a member of the type
	// instance should be of the same type as the meta_type
	virtual void* walk_ptr(void* instance) const = 0;

	// calls meta_serial_write with the correct templated type
	// instance should already be walked...
	virtual void serial_write(serial_writer* serial, void* instance) const = 0;

	// calls meta_serial_read with the correct templated type
	// instance should already be walked...
	virtual void serial_read(serial_reader* serial, void* instance) const = 0;
};

// this is a map representation of a type that any serializer can use
// this way there doesnt need to be so many tree marching functions in the meta_type itself

struct meta_walker
{
	void* m_instance;
	meta_type* m_type;
	std::unordered_map<std::string, meta_walker*> m_children;

	meta_walker(
		meta_type* type,
		void* instance
	)
		: m_type     (type)
		, m_instance (instance)
	{
		for (meta_type* child : type->get_members())
		{
			m_children[child->member_name()] = new meta_walker(child, child->walk_ptr(instance));
		}
	}

	~meta_walker()
	{
		for (const auto& [_, child] : m_children)
		{
			delete child;
		}
	}

	const meta_walker& get(const std::string& name) const
	{
		return *m_children.at(name);
	}

	const meta_walker& operator[](const std::string& name) const
	{
		return get(name);
	}

	void* value() const
	{
		return m_instance;
	}

	void walk(std::function<void(meta_type*, void*)> walker) const
	{
		walker(m_type, m_instance);

		for (const auto& [_, child] : m_children)
		{
			child->walk(walker);
		}
	}
};

// Serial

struct serial_writer
{
	std::ostream& m_out;
	bool m_binary;

	serial_writer(std::ostream& out, bool binary)
		: m_out    (out)
		, m_binary (binary)
	{}

	// this should recurse down the tree of type

	virtual void write(meta_type* type, void* instance) = 0;

	// these two are special conditions

	virtual void write_length(size_t length) = 0;

	virtual void write_array(meta_type* type, void* instance, size_t repeat) = 0;
	virtual void write_string(const char* str, size_t length) = 0;

	template<typename _t>
	void write(const _t& value)
	{
		if (m_binary)
		{
			write_string((const char*)&value, sizeof(_t));
		}

		else if constexpr (std::is_integral<_t>::value)
		{
			m_out << value;
		}
	}
};

struct serial_reader
{
	std::istream& m_in;
	bool m_binary;

	serial_reader(std::istream& in, bool binary)
		: m_in     (in)
		, m_binary (binary)
	{}

	// this should recurse down the tree of type

	virtual void read(meta_type* type, void* instance) = 0;

	// these two are special conditions

	virtual size_t read_length() = 0;

	virtual void read_array(meta_type* type, void* instance, size_t repeat) = 0;
	virtual void read_string(char* str, size_t length) = 0;

	template<typename _t>
	void read(_t& value)
	{
		if (m_binary)
		{
			read_string((char*)&value, sizeof(_t));
		}

		else if constexpr (std::is_integral<_t>::value)
		{
			m_in >> value;
		}
	}
};

// specialize these for custom types / containers

template<typename _t>
void meta_serial_write(meta_tag<_t>, serial_writer* serial, const _t& value)
{
	serial->write<_t>(value);
}

template<typename _t>
void meta_serial_read(meta_tag<_t>, serial_reader* serial, _t& value)
{
	serial->read<_t>(value);
}

struct bin_writer : serial_writer
{ 
	bin_writer(std::ostream& out)
		: serial_writer(out, true)
	{}

	void write(meta_type* type, void* instance) override
	{
		auto& members = type->get_members();

		if (members.size() > 0)
		{
			for (int i = 0; i < members.size(); i++)
			{
				meta_type* member = members.at(i);
				write(member, member->walk_ptr(instance));
			}
		}

		else
		{
			type->serial_write(this, instance);
		}
	}

	void write_length(size_t length) override
	{
		serial_writer::write<size_t>(length);
	}

	void write_array(meta_type* type, void* instance, size_t repeat) override
	{
		for (int i =  0; i < repeat; i++)
		{
			write(type, (char*)instance + i * type->m_info->m_size);
		}
	}

	void write_string(const char* str, size_t length) override
	{
		const char* end = str + length;
		while (str != end)
		{
			m_out << std::setw(2)
					<< std::setfill('0')
					<< std::hex
					<< (int)*str;

			++str;
		}
	}
};

struct bin_reader : serial_reader
{ 
	bin_reader(std::istream& in)
		: serial_reader (in, true)
	{}

	void read(meta_type* type, void* instance) override
	{
		auto& members = type->get_members();

		if (members.size() > 0)
		{
			for (int i = 0; i < members.size(); i++)
			{
				meta_type* member = members.at(i);
				read(member, member->walk_ptr(instance));
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
		serial_reader::read<size_t>(length);
		return length;
	}

	void read_array(meta_type* type, void* instance, size_t repeat) override
	{
		for (int i =  0; i < repeat; i++)
		{
			read(type, (char*)instance + i * type->m_info->m_size);
		}
	}

	void read_string(char* str, size_t length) override
	{
		const char* end = str + length;
		while (str != end)
		{
			m_in >> std::setw(2)
				 >> std::setfill('0')
				 >> std::hex
				 >> *str;

			++str;
		}
	}
};

struct json_writer : serial_writer
{
	json_writer(std::ostream& out)
		: serial_writer(out, false)
	{}

	void write(meta_type* type, void* instance) override
	{
		auto& members = type->get_members();

		if (members.size() > 0) // objects
		{
			m_out << '{';
			for (int i = 0; i < members.size(); i++)
			{
				meta_type* member = members.at(i);
				m_out << '"' << member->member_name() << '\"' << ':';
				write(member, member->walk_ptr(instance));

				if (i != members.size() - 1)
				{
					m_out << ',';
				}
			}
			m_out << '}';
		}

		else
		{
			type->serial_write(this, instance);
		}
	}

	void write_length(size_t length) override
	{
		// do nothing
	}

	void write_array(meta_type* type, void* instance, size_t repeat) override
	{
		m_out << '[';
		for (int i =  0; i < repeat; i++)
		{
			write(type, (char*)instance + i * type->m_info->m_size);

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

// internal

template<typename _t>
struct meta_class : meta_type
{
	std::vector<meta_type*> m_members;

	meta_class(
		meta_type_info* info
	)
		: meta_type (info)
	{}

	bool is_member() const override
	{
		return false;
	}

	const std::vector<meta_type*>& get_members() const override
	{
		return m_members;
	}

	const std::string& member_name() const override
	{
		assert(false && "meta_type was not a member");
		throw nullptr;
	}

	void* walk_ptr(void* instance) const override
	{
		return instance;
	}

	void serial_write(serial_writer* serial, void* instance) const override
	{
		meta_serial_write(meta_tag<_t>{}, serial, *(_t*)instance);
	}

	void serial_read(serial_reader* serial, void* instance) const override
	{
		meta_serial_read(meta_tag<_t>{}, serial, *(_t*)instance);
	}
};

template<typename _t, auto _m>
struct meta_class_member : meta_type
{
	using _mtype = ptrtype<_m>;

	meta_class<_mtype>* m_class; // doesnt own
	std::string m_name_in_class;

	meta_class_member(
		meta_type_info* info,
		meta_class<_mtype>* class_,
		std::string name_in_class
	)
		: meta_type       (info)
		, m_class         (class_)
		, m_name_in_class (name_in_class)
	{}

	bool is_member() const override
	{
		return true;
	}

	const std::vector<meta_type*>& get_members() const override
	{
		return m_class->get_members();
	}

	const std::string& member_name() const override
	{
		return m_name_in_class;
	}

	void* walk_ptr(void* instance) const override
	{
		return & ( ((_t*)instance)->*_m );
	}

	void serial_write(serial_writer* serial, void* instance) const override
	{
		meta_serial_write(meta_tag<_mtype>{}, serial, *(_mtype*)instance);
	}

	void serial_read(serial_reader* serial, void* instance) const override
	{
		meta_serial_read(meta_tag<_mtype>{}, serial, *(_mtype*)instance);
	}
};

struct meta
{
	template<typename _t>
	static meta_type_info* get_type_info()
	{
		static meta_type_info* info = make_type_info<_t>();
		return info;
	}

	template<typename _t>
	static meta_class<_t>* get_class()
	{
		static meta_class<_t>* type = make_class<_t>();
		return type;
	}

private:
	template<typename _t>
	static meta_type_info* make_type_info()
	{
		meta_type_info* info = new meta_type_info();
		info->m_name = meta_typename(meta_tag<_t>{});
		info->m_size = sizeof(_t);
		
		return info;
	}

	template<typename _t>
	static meta_class<_t>* make_class()
	{
		meta_class<_t>* type = new meta_class<_t>(get_type_info<_t>());
		return type;
	}
};

template<typename _t>
struct meta_class_builder
{
	meta_class<_t>* m_current = meta::get_class<_t>();

	meta_class_builder<_t>& name(const std::string& name)
	{
		m_current->m_info->m_name = name;
		return *this;
	}

	template<auto _m>
	meta_class_builder<_t>& member(const std::string& name)
	{
		meta_type_info* member_info = meta::get_type_info<ptrtype<_m>>();
		meta_class<ptrtype<_m>>* member_class = meta::get_class<ptrtype<_m>>();

		m_current->m_members.push_back(new meta_class_member<_t, _m>(
			member_info, member_class, name
		));

		return *this;
	}

	meta_class<_t>* get() const
	{
		return m_current;
	}
};

inline
void __debug_print_memory(void* address, size_t columns, size_t rows)
{
	for (size_t j = 0; j < rows; j++)
	{
		char* addr = (char*)address + columns * j;

		printf("\n 0x%p |", addr);
		for (size_t i = 0; i < columns; i++)
		{
			printf(" %02hhx", *(addr + i));
		}

		printf(" | ");
		for (size_t i = 0; i < columns; i++)
		{
			char c = *(addr + i);
			if (c > 31 && c < 127) printf("%c", c);
			else                   printf("_");
		}
	}
}