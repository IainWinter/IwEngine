#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <ostream>
#include <assert.h>

// ! This doesnt support pointers !
// wrap in a std container and provide custom behaviour to your needs

#include "util/type_info.h"

namespace meta
{
	//
	// serial forward declare
	//

	struct serial_writer;
	struct serial_reader;

	struct type
	{
		type_info* m_info;
		
		type(
			type_info* info
		)
			: m_info (info)
		{}

		virtual bool is_member() const = 0;

		// asserts m_has_members = true
		virtual const std::vector<type*>& get_members() const = 0;

		// asserts m_is_member = true
		virtual const std::string& member_name() const = 0;

		// walks a pointer forward to a member of the type
		// instance should be of the same type as the type
		virtual void* walk_ptr(void* instance) const = 0;

		// calls serial_write with the correct templated type
		// instance should already be walked...
		virtual void _serial_write(serial_writer* serial, void* instance) const = 0;

		// calls serial_read with the correct templated type
		// instance should already be walked...
		virtual void _serial_read(serial_reader* serial, void* instance) const = 0;
		
		// helpers

		bool has_members() const
		{
			return get_members().size() > 0;
		}
	};

	template<typename _t>
	type* get_class();

	// this is a map representation of a type instance

	struct walker
	{
		void* m_instance;
		type* m_type;
		std::unordered_map<std::string, walker*> m_children;

		walker(
			type* type,
			void* instance
		)
			: m_type     (type)
			, m_instance (instance)
		{
			for (meta::type* child : type->get_members())
			{
				m_children[child->member_name()] = new walker(child, child->walk_ptr(instance));
			}
		}

		~walker()
		{
			for (const auto& [_, child] : m_children)
			{
				delete child;
			}
		}

		const walker& get(const std::string& name) const
		{
			return *m_children.at(name);
		}

		const walker& operator[](const std::string& name) const
		{
			return get(name);
		}

		void* value() const
		{
			return m_instance;
		}

		void walk(std::function<void(type*, void*)> walker) const
		{
			walker(m_type, m_instance);

			for (const auto& [_, child] : m_children)
			{
				child->walk(walker);
			}
		}
	};

	//
	// serialization
	//

	struct serial_writer
	{
		std::ostream& m_out;
		bool m_binary;

		serial_writer(std::ostream& out, bool binary)
			: m_out    (out)
			, m_binary (binary)
		{}

		// this should recurse down the tree of type

		virtual void write_type(type* type, void* instance) = 0;

		// these two are special conditions

		virtual void write_length(size_t length) = 0;
		virtual void write_array(type* type, void* instance, size_t repeat) = 0;
		virtual void write_string(const char* str, size_t length) = 0;

		template<typename _t>
		void write_value(const _t& value)
		{
			if (m_binary)
			{
				write_string((const char*)&value, sizeof(_t));
			}

			else if constexpr (std::is_integral<_t>::value || std::is_floating_point<_t>::value) // should just test for << op
			{
				m_out << value;
			}

			// should assert
			// this is where types without a serial_write override come

			// could rework above, its a catch all for simple types for text based outputs
		}

		template<typename _t>
		void write(const _t& value)
		{
			write_type(get_class<_t>(), (void*)&value);
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

		virtual void read_type(type* type, void* instance) = 0;

		// these two are special conditions

		virtual size_t read_length() = 0;
		virtual void read_array(type* type, void* instance, size_t repeat) = 0;
		virtual void read_string(char* str, size_t length) = 0;

		template<typename _t>
		void read_value(_t& value)
		{
			if (m_binary)
			{
				read_string((char*)&value, sizeof(_t));
			}

			else if constexpr (std::is_integral<_t>::value || std::is_floating_point<_t>::value) // should just test for << op
			{
				m_in >> value;
				// this cant be here because of json.h needs to read the values
				// reader seems to be able to handle the input in a way to never call this
			}

			// could assert for not reading
		}

		template<typename _t>
		void read(_t& value)
		{
			read_type(meta::get_class<_t>(), &value);
		}
	};

	// default behaviour is to just write the value
	// specialize these for custom types

	template<typename _t>
	void serial_write(serial_writer* serial, const _t& value)
	{
		serial->write_value(value);
	}

	template<typename _t>
	void serial_read(serial_reader* serial, _t& value)
	{
		serial->read_value(value);
	}

namespace internal
{
	template<typename _t>
	struct class_type : type
	{
		std::vector<type*> m_members;

		class_type(
			type_info* info
		)
			: type (info)
		{}

		bool is_member() const override
		{
			return false;
		}

		const std::vector<type*>& get_members() const override
		{
			return m_members;
		}

		const std::string& member_name() const override
		{
			assert(false && "type was not a member");
			throw nullptr;
		}

		void* walk_ptr(void* instance) const override
		{
			return instance;
		}

		void _serial_write(serial_writer* serial, void* instance) const override
		{
			serial_write(serial, *(_t*)instance);
		}

		void _serial_read(serial_reader* serial, void* instance) const override
		{
			serial_read(serial, *(_t*)instance);
		}
	};

	template<typename _t, auto _m>
	struct class_member : type
	{
		using _mtype = ptrtype<_m>;

		class_type<_mtype>* m_class;
		std::string m_name;

		class_member(
			class_type<_mtype>* member_class_type,
			std::string member_name
		)
			: type    (member_class_type->m_info)
			, m_class (member_class_type)
			, m_name  (member_name)
		{}

		bool is_member() const override
		{
			return true;
		}

		const std::vector<type*>& get_members() const override
		{
			return m_class->get_members();
		}

		const std::string& member_name() const override
		{
			return m_name;
		}

		void* walk_ptr(void* instance) const override
		{
			return & ( ((_t*)instance)->*_m );
		}

		void _serial_write(serial_writer* serial, void* instance) const override
		{
			serial_write(serial, *(_mtype*)instance);
		}

		void _serial_read(serial_reader* serial, void* instance) const override
		{
			serial_read(serial, *(_mtype*)instance);
		}
	};

	template<typename _t>
	class_type<_t>* get_class()
	{
		static class_type<_t> type(get_type_info<_t>());
		return &type;
	}

	template<typename _t, auto _m>
	class_member<_t, _m>* make_member(const std::string& name)
	{
		static class_member<_t, _m> member(get_class<ptrtype<_m>>(), name);
		return &member;
	}
}

	template<typename _t>
	struct describe
	{
		internal::class_type<_t>* m_current = internal::get_class<_t>();

		describe<_t>& name(const std::string& name)
		{
			m_current->m_info->m_name = name;
			return *this;
		}

		template<auto _m>
		describe<_t>& member(const std::string& name)
		{
			m_current->m_members.push_back(internal::make_member<_t, _m>(name));
			return *this;
		}

		type* get() const
		{
			return m_current;
		}
	};

	template<typename _t>
	type* get_class()
	{
		return internal::get_class<_t>();
	}

// should add a function that can describe templated types
// would get called like get type name

// template<typename _t>
// void describe_type(tag<_t>)
// {
// 	return typeid(_t).name();
// }
}

