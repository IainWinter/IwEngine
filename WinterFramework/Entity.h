#pragma once

#include "util/pool_allocator.h"
#include <initializer_list>
#include <unordered_map>
#include <type_traits>
#include <functional>
#include <algorithm>
#include <assert.h>
#include <tuple>
#include <stdint.h>

#define ENTITY_USE_SERIAL

#ifdef ENTITY_USE_SERIAL
#	include "Serial.h"
#else
#	include "util/type_info.h"
#endif

using hash_t = uint64_t;
using rep_t  = uint16_t; // low bits of the hash to represent repeat components in archetype 

using hash_bits = std::integral_constant<hash_t, ~(hash_t)rep_t(-1) & hash_t(-1)>;
using rep_bits  = std::integral_constant<hash_t,  (hash_t)rep_t(-1)>;

constexpr void set_repeats(hash_t& hash, rep_t count) { hash = (hash & hash_bits::value) | (hash_t)count; }
constexpr rep_t get_repeats(const hash_t& hash) { return (rep_t)(hash & rep_bits::value); }
constexpr hash_t just_hash(const hash_t& hash) { return hash & hash_bits::value; }

#ifndef max // Windows.h and other system headers define these
	template<typename _t> constexpr _t max(_t a, _t b) { return a > b ? a : b; }
	template<typename _t> constexpr _t min(_t a, _t b) { return a < b ? a : b; }
#endif

struct component
{
#ifdef ENTITY_USE_SERIAL
	meta::type* m_type = nullptr;
#endif

	meta::type_info* m_info = nullptr;
	size_t m_hash = 0; // this needs to not be in the type_info because it changes to track repeats
	
	std::function<void(void*)>        m_destructor;
	std::function<void(void*)>        m_default;
	std::function<void(void*, void*)> m_move;
};

template<typename _t>       // this should be the only place that templates are necessary for storage
component make_component()  //	everyother template function is short-hand back to this
{					  
	static component c;// dll bounds..., could remove 'static'

	if (c.m_hash == 0)
	{
#ifdef ENTITY_USE_SERIAL
		c.m_type = meta::get_class<_t>();
#endif

		c.m_info = meta::get_type_info<_t>();
		c.m_hash = just_hash(typeid(_t).hash_code());

		c.m_destructor = [](void* ptr)             { ((_t*)ptr)->~_t(); };
		c.m_default    = [](void* ptr)             { new (ptr) _t(); };
		c.m_move       = [](void* ptr, void* data) { new (ptr) _t(std::move(*(_t*)data)); };
	}
	return c;
}

// This is a helper for creating archetypes dynamicly

struct component_list
{
	std::vector<component> m_list;

	component_list(std::initializer_list<component> components)
		: m_list(components)
	{}

	operator std::vector<component>& () { return m_list; }
	operator const std::vector<component>&() const { return m_list; }

	void add(const component& component)
	{
		m_list.push_back(component);
	}

	void remove(const component& component)
	{
		for (auto itr = m_list.begin(); itr != m_list.end(); ++itr)
		{
			if (itr->m_hash == component.m_hash)
			{
				m_list.erase(itr);
				break;
			}
		}
	}

	template<typename _t>
	void add() { add(make_component<_t>()); }

	template<typename _t>
	void remove() { remove(make_component<_t>()); }
};

template<typename... _t>
component_list wrap()
{
	return component_list { make_component<_t>()... };
}

struct archetype
{
	hash_t m_hash = 0;
	int m_size    = 0;
	std::vector<component> m_components;
	std::vector<int> m_offsets;
};

inline
archetype make_archetype(
	const std::vector<component>& components)
{
	archetype a = { 0, 0 };
	a.m_components = components;
	a.m_offsets.reserve(a.m_components.size());

	std::sort(
		a.m_components.begin(),
		a.m_components.end(),
		[](const auto& a, const auto& b)
		{ 
			return a.m_hash > b.m_hash;
		}
	);

	for (auto c = a.m_components.begin(); c != a.m_components.end(); ++c)
	{
		a.m_offsets.push_back(a.m_size);
		a.m_size += c->m_info->m_size;
	}

	// count backwards for repeats
	std::unordered_map<hash_t, rep_t> counts;
	for (auto c = a.m_components.rbegin(); c != a.m_components.rend(); ++c)
	{
		set_repeats(c->m_hash, counts[just_hash(c->m_hash)]++);
		a.m_hash ^= (c->m_hash + 0x9e3779b9) + (a.m_hash << 6);
	}

	return a;
}

inline
archetype archetype_add(
	const archetype& archetype,
	const std::vector<component>& to_add)
{
	std::vector<component> components = archetype.m_components;
	components.insert(components.end(), to_add.begin(), to_add.end());
	return make_archetype(components);
}

inline
archetype archetype_remove(
	const archetype& archetype,
	const std::vector<component>& to_remove)
{
	std::vector<component> components = archetype.m_components;

	for (int i = 0; i < components.size(); i++)
	{
		for (const component& c : to_remove)
		{
			if (components.at(i).m_hash == c.m_hash)
			{
				components.at(i) = components.back(); components.pop_back();
				i--;
			}
		}
	}

	return make_archetype(components);
}

inline
bool is_in_archetype(
	const archetype& archetype,
	const component& component)
{
	for (const ::component& c : archetype.m_components)
	{
		if (c.m_hash == component.m_hash)
		{
			return true;
		}
	}

	return false;
}

// Currently components are stored in an AOS format
// Entity1|Component1|Component2|Entity2|Component1|Component2
// Havent tested which is better for cache could store SOA
// Entity1|Entity2|Component1|Component1|Component2|Component2

// todo: find a way to profile this...

inline
int offset_of_component(
	const archetype& archetype,
	const component& component)
{
	for (int i = 0; i < archetype.m_components.size(); i++)
	{
		const ::component& c = archetype.m_components.at(i);

		if (just_hash(c.m_hash) == just_hash(component.m_hash))
		{
			return archetype.m_offsets.at(i) + c.m_info->m_size * get_repeats(component.m_hash);
		}

		else
		{
			i += get_repeats(c.m_hash);
		}
	}

	assert(false && "Component not in archetype");
	return -1;
}

template<typename... _t>
archetype make_archetype()
{
	return make_archetype({ make_component<_t>()... });
}

template<typename... _t>
archetype add_to_archetype(
	const archetype& archetype)
{
	return add_to_archetype(archetype, { make_component<_t>()... });
}

// querying archetypes for components

template<int _s>
std::array<hash_t, _s> make_query(
	const std::array<hash_t, _s>& components)
{
	std::array<hash_t, _s> hashes;

	std::unordered_map<hash_t, rep_t> counts;
	for (int i = 0; i < _s; i++)
	{
		hash_t& hash = hashes.at(i);
		hash = components.at(i);
		set_repeats(hash, counts[just_hash(hash)]++);
	}

	return hashes;
}

template<typename... _q>
std::array<int, sizeof...(_q)> query_offsets(
	const archetype& archetype)
{
	return { offset_of_component(archetype, make_component<_q>())... };
}

template<typename... _q>
std::array<hash_t, sizeof...(_q)> make_query()
{
	return make_query<sizeof...(_q)>({ make_component<_q>().m_hash ... });
}

template<int _s>
bool is_query_match(
	const archetype& archetype,
	const std::array<uint64_t, _s>& hashes)
{
	bool contains_all = true;
	for (hash_t hash : hashes)
	{
		bool contains = false;
		for (const component& c : archetype.m_components)
		{
			if (hash == c.m_hash)
			{
				contains = true;
				break;
			}
		}

		if (!contains)
		{
			contains_all = false;
			break;
		}
	}

	return contains_all;
}

// mapping raw pointers to references to query results

template<typename... _t, size_t... _index>
std::tuple<_t&...> tie_from_offsets_i(
	void* raw,
	const std::array<int, sizeof...(_t)>& offsets,
	std::index_sequence<_index...>)
{
	static_assert(sizeof...(_t) == sizeof...(_index));
	return std::tuple<_t&...>(*(_t*)((char*)raw + offsets[_index]) ...);
}

template<typename... _t>
std::tuple<_t&...> tie_from_offsets(
	void* raw,
	const std::array<int, sizeof...(_t)>& offsets)
{
	return tie_from_offsets_i<_t...>(raw, offsets, std::make_index_sequence<sizeof...(_t)>{});
}

// does 8byte-8byte-4bytes align to 24?

struct entity_handle
{
	size_t m_index     = 0;
	int    m_version   = 0;
	hash_t m_archetype = 0;

	hash_t hash() const { return m_archetype + hash_t(m_index); } // no need for real hash
};

struct entity_data
{
	void* m_components; // ptr to the first component in archetype
	int m_version;      // for if entity coming in is stale

	size_t m_index;        // annoying but for itr to return entity_handle without a search through the pool's pages
};

struct entity_storage
{
	using itr = pool_iterator;
	itr get_itr() { return pool_iterator(m_pool); }

	archetype m_archetype;
	pool_allocator m_pool;
	int m_instance;
	int m_count;   // number of alive entities

	entity_storage(
		const archetype& archetype
	)
		: m_archetype (archetype)
		, m_pool      (16)
		, m_instance  (0)
		, m_count     (0)
	{
		m_pool.m_block_size = archetype.m_size + sizeof(entity_data);
	}

	entity_handle create_entity()
	{
		char* raw = m_pool.alloc_block();

		entity_data* entity = (entity_data*)raw;
		entity->m_components = raw + sizeof(entity_data);
		entity->m_version = m_instance;
		entity->m_index = m_pool.get_block_index(raw); // search, alloc_block could return this

		for (const component& component : m_archetype.m_components)
		{
			component.m_default(offset_raw_pointer(entity->m_components, component));
		}

		m_count += 1;
		m_instance += 1;

		return wrap(entity);
	}

	void destroy_entity(
		entity_handle handle,
		bool destroy_components = true)
	{
		assert(handle.m_archetype == m_archetype.m_hash && "entity_storage::destroy_entity failed, archetype mismatch");
		assert(handle.m_version == unwrap(handle)->m_version && "entity_storage::destroy_entity failed, version mismatch");

		entity_data* entity = unwrap(handle);

		if (destroy_components)
		{
			for (const component& component : m_archetype.m_components)
			{
				component.m_destructor(offset_raw_pointer(entity->m_components, component));
			}
		}
		
		m_pool.free_block(entity);
		m_count -= 1;
	}

	bool is_entity_alive(
		entity_handle handle) const
	{
		assert(handle.m_archetype == m_archetype.m_hash && "entity_storage::is_entity_alive failed, archetype mismatch");
		
		entity_data* entity = unwrap(handle);

		return m_pool.has_allocated(entity)
			&& handle.m_version == entity->m_version;
	}

	std::pair<bool, entity_handle> find_from_component(
		const component& component,
		void* component_addr) const
	{
		if (m_pool.has_allocated(component_addr))
		{
			// Entity data is stored before the first component
			// see offset_of_component, that would change this code 

			void* raw = (char*)component_addr - offset_of_component(m_archetype, component) - sizeof(entity_data);
			return std::make_pair(true, wrap((entity_data*)raw));
		}

		return std::make_pair(false, entity_handle {});
	}

	entity_handle move_entity(
		entity_handle handle,
		entity_storage& new_store)
	{
		assert(handle.m_archetype == m_archetype.m_hash      && "entity_storage::move_entity failed, archetype mismatch");
		assert(handle.m_version == unwrap(handle)->m_version && "entity_storage::move_entity failed, version mismatch");

		entity_handle new_handle = new_store.create_entity();

		entity_data* old_entity =           unwrap(handle);
		entity_data* new_entity = new_store.unwrap(new_handle);

		void* old_data = old_entity->m_components;
		void* new_data = new_entity->m_components;

		int old_length =           m_archetype.m_components.size();
		int new_length = new_store.m_archetype.m_components.size();
		int max_length = max(old_length, new_length);

		// this needs to find the first index that both match
		// then could linearly loop this
		//for (int i = 0; i < max_length; i++)
		//{
		//	int o = min(i, old_length - 1);
		//	int n = min(i, new_length - 1);
		for (int o = 0; o < old_length; o++)
		for (int n = 0; n < new_length; n++) // might be able to start at o
		{
			const component& old_component =           m_archetype.m_components.at(o);
			const component& new_component = new_store.m_archetype.m_components.at(n);

			if (old_component.m_hash == new_component.m_hash)
			{
				old_component.m_move(
					new_store.offset_raw_pointer(new_data, new_component),
					          offset_raw_pointer(old_data, old_component)
				);

				break; // iterate unique pairs only, verify this
			}
		}

		destroy_entity(handle, false);

		return new_handle;
	}

	void set_component(
		entity_handle handle,
		const component& component,
		void* data)
	{
		assert(handle.m_archetype == m_archetype.m_hash      && "entity_storage::set_component failed, archetype mismatch");
		assert(handle.m_version == unwrap(handle)->m_version && "entity_storage::set_component failed, version mismatch");
		assert(data                                          && "entity_storage::set_component failed, data is null");

		component.m_move(get_raw_pointer(handle, component), data);
	}

	void* get_raw_pointer(
		entity_handle handle,
		const component& component) const
	{
		assert(handle.m_archetype == m_archetype.m_hash      && "entity_storage::get_raw_pointer failed, archetype mismatch");
		assert(handle.m_version == unwrap(handle)->m_version && "entity_storage::get_raw_pointer failed, version mismatch");

		return offset_raw_pointer(unwrap(handle)->m_components, component);
	}

	void* offset_raw_pointer(
		void* ptr,
		const component& component) const
	{
		return (char*)ptr + offset_of_component(m_archetype, component);
	}

	entity_handle wrap(const entity_data* data) const
	{
		return { data->m_index, data->m_version, m_archetype.m_hash };
	}

	entity_data* unwrap(const entity_handle& handle) const
	{
		return (entity_data*)m_pool.get_block_address(handle.m_index);
	}
};

struct entity_manager; // needed for destroy / add. Breaks flow a little bit :(
				   // could just reutrn entity_handles, but that was annoying in previous versions

struct entity
{
	entity_handle   m_handle;
	entity_storage* m_store   = nullptr;
	entity_manager* m_manager = nullptr;

	bool operator==(const entity& other) const { return m_handle.hash() == other.m_handle.hash(); }
	bool operator!=(const entity& other) const { return !operator==(other); }

	void destroy(); // defined under entity_manager

	bool is_alive() const
	{
		if (m_handle.m_archetype == hash_t(0))
		{
			return false;
		}

		assert(m_store && "entity::is_alive failed, no store");
		return m_store->is_entity_alive(m_handle);
	}

	template<typename _t>
	_t& get() const
	{
		assert(m_store && "entity::get failed, no store");
		return *(_t*)m_store->get_raw_pointer(m_handle, make_component<_t>());
	}

	template<typename _t>
	bool has() const
	{
		assert(m_store && "entity::has failed, no store");
		return is_in_archetype(m_store->m_archetype, make_component<_t>());
	}

	template<typename _t, typename... _args>
	entity& set(_args&&... args);

	template<typename _t, typename... _args>
	entity& add(_args&&... args);

	template<typename _t>
	entity& remove();

	// listen

	entity& on_destroy(const std::function<void(entity)>                  & func); // defined under entity_manager
	entity& on_set    (const std::function<void(entity)>                  & func);
	entity& on_set    (const std::function<void(entity, const component&)>& func);
	entity& on_add    (const std::function<void(entity)>                  & func);
	entity& on_add    (const std::function<void(entity, const component&)>& func);
	entity& on_remove (const std::function<void(entity)>                  & func);
	entity& on_remove (const std::function<void(entity, const component&)>& func);
};

// should make a version that returns void* and then this just encapsulates that and casts

template<bool _with_entity, typename... _q>
struct entity_query
{
	using result_t = typename std::conditional<_with_entity,
		std::tuple<entity, _q&...>,
		std::tuple<        _q&...>>::type;

	struct itr
	{
		struct itr_part
		{
			entity_storage* m_store; // this is ONLY to return an 'entity', ONLY needs archetype hash from it...
			entity_storage::itr m_itr;
			std::array<int, sizeof...(_q)> m_offsets;
		};

		entity_manager* m_manager; // ref to manager, ONLY for returning entity instead of entity_handle
		
		std::vector<itr_part> m_parts;
		typename std::vector<itr_part>::iterator m_current;
		int m_more;

		itr(
			entity_manager* manager,
			std::vector<entity_storage*> matches
		)
			: m_manager (manager)
			, m_more    (0)
		{
			// If its a begin iterator, and its not empty, add it to parts
			// If its an end iterator, and there is a back and its not empty, add it to parts
			// If parts is empty, add a null iterator to fake end

			for (entity_storage* storage : matches)
			{
				if (storage->m_count == 0) continue;
				m_parts.emplace_back(itr_part {
					storage,
					storage->get_itr(),
					query_offsets<_q...>(storage->m_archetype)
				});

				m_more += 1;
			}

			if (m_parts.size() == 0)
			{
				m_parts.emplace_back();
			}

			m_current = m_parts.begin();
		}

		std::tuple<_q&...> just_components() const
		{
			entity_data* data = m_current->m_itr.template get<entity_data>();

			return tie_from_offsets<_q...>(
				data->m_components,
				m_current->m_offsets
			);
		}

		result_t operator*() const
		{
			if constexpr (_with_entity)
			{
				entity_data* data = m_current->m_itr.template get<entity_data>();
				
				entity_handle handle = {
					data->m_index,
					data->m_version,
					m_current->m_store->m_archetype.m_hash
				};

				return std::tuple_cat(
					std::make_tuple<entity>({handle, m_current->m_store, m_manager}),
					just_components()
				);
			}

			else
			{
				return just_components();
			}
		}

		itr& operator++()
		{
			m_current->m_itr.next();
			if (!m_current->m_itr.more())
			{
				--m_more;
				if (m_more > 0) ++m_current;
			}

			return *this;
		}

		bool operator==(const itr& other) const 
		{
			return m_more == other.m_more
				&& m_current->m_itr.more() == other.m_current->m_itr.more(); // note more check if correct
		}

		bool operator!=(const itr& other) const
		{
			return !operator==(other);
		}
	};

	std::vector<entity_storage*> m_matches;
	entity_manager* m_manager;

	// only move
	entity_query(entity_manager* manager) : m_manager(manager) {}
	entity_query(const entity_query& copy) = delete;
	entity_query& operator=(const entity_query& copy) = delete;

	entity_query(entity_query&& move) = default;
	entity_query& operator=(entity_query&& move) = default;

	int count() const
	{
		int c = 0;
		for (entity_storage* store : m_matches) c += store->m_count;
		return c;
	}

	itr begin() { return itr(m_manager, m_matches); }
	itr end()   { return itr(m_manager, {});  }

	// these cant be referemces cus the thing pops off the stack, might need std::move bc I think these are copying the vector

	entity_query<true>        only_entity() { return std::move(*(entity_query<true       >*)this); }
	entity_query<true, _q...> with_entity() { return std::move(*(entity_query<true, _q...>*)this); }

	template<typename... _o>
	entity_query<_with_entity, _o...> only() { return std::move(*(entity_query<_with_entity, _o...>*)this); }

	void for_each(const std::function<void(entity, _q&...)>& func)
	{
		for (auto& tuple : with_entity())
		{
			std::apply(func, tuple);
		}
	}

	void for_each(const std::function<void(_q&...)>& func)
	{
		for (auto& tuple : *this)
		{
			std::apply(func, tuple);
		}
	}

	result_t first()
	{
		return *begin();
	}
};

// for events and command_buffer

enum class entity_command
{
	CREATE,
	DESTROY,
	SET,
	ADD,
	REMOVE,
};

struct entity_manager
{
	std::unordered_map<hash_t, entity_storage> m_storage;

	entity create(const archetype& archetype)
	{
		entity_handle handle = get_storage(archetype).create_entity();
		return wrap(handle);
	}

	void destroy(entity_handle handle)
	{
		call_listener(entity_command::DESTROY, handle, nullptr);
		remove_listeners(handle);
		entity_storage& store = m_storage.at(handle.m_archetype);
		store.destroy_entity(handle);
		clean_storage(store); // breaks itr
	}

	bool is_alive(entity_handle handle)
	{
		return m_storage.at(handle.m_archetype).is_entity_alive(handle);
	}

	std::pair<bool, entity> find(const component& component, void* ptr_to_component)
	{
		for (const std::pair<hash_t, entity_storage>& store : m_storage) // could use query if it didnt need types... (ezish fix)
		{
			if (is_in_archetype(store.second.m_archetype, component))
			{
				std::pair<bool, entity_handle> out = store.second.find_from_component(component, ptr_to_component);

				if (out.first)
				{
					return std::make_pair(true, wrap(out.second));
				}
			}
		}

		return std::make_pair(false, entity{});
	}

	void set(entity_handle handle, const component& component, void* data)
	{
		m_storage.at(handle.m_archetype).set_component(handle, component, data);
		call_listener(entity_command::SET, handle, &component);
	}

	entity add(entity_handle handle, const component& component, void* data = nullptr)
	{
		entity_storage& old_store = m_storage.at(handle.m_archetype);
		entity_storage& new_store = get_storage(archetype_add(old_store.m_archetype, { component }));

		entity_handle new_handle = old_store.move_entity(handle, new_store);
		new_store.set_component(new_handle, component, data);

		call_listener(entity_command::ADD, handle, &component);

		clean_storage(old_store); // breaks itr
		return wrap(new_handle);
	}

	entity remove(entity_handle handle, const component& component)
	{
		entity_storage& old_store = m_storage.at(handle.m_archetype);
		entity_storage& new_store = get_storage(archetype_remove(old_store.m_archetype, { component }));

		call_listener(entity_command::REMOVE, handle, &component);

		entity_handle new_handle = old_store.move_entity(handle, new_store);

		clean_storage(old_store); // breaks itr
		return wrap(new_handle);
	}

	void get(entity_handle handle, const component& component, void** out)
	{
		*out = m_storage.at(handle.m_archetype).get_raw_pointer(handle, component);
	}

	bool has(entity_handle handle, const component& component)
	{
		return is_in_archetype(m_storage.at(handle.m_archetype).m_archetype, component);
	}

	// helpers

	entity_storage& get_storage(const archetype& archetype)
	{
		auto itr = m_storage.find(archetype.m_hash);
		if (itr == m_storage.end())
		{
			itr = m_storage.emplace_hint(itr, archetype.m_hash, entity_storage(archetype));
		}

		return itr->second;
	}

	entity wrap(entity_handle handle)
	{
		return entity { handle, &m_storage.at(handle.m_archetype), this };
	}

	// template helpers

	template<typename... _t>
	entity create()
	{
		return create(make_archetype<_t...>());
	}

	template<typename _t>
	std::pair<bool, entity> find(_t* ptr_to_component)
	{
		return find(make_component<_t>(), ptr_to_component);
	}

	template<typename _t, typename... _args>
	void set(entity_handle handle, _args&&... args)
	{
		_t temp = _t{args...}; // this gets moved
		set(handle, make_component<_t>(), &temp);
	}

	template<typename _t, typename... _args>
	entity add(entity_handle handle, _args&&... args)
	{
		if constexpr (sizeof...(_args) == 0) {
			return add(handle, make_component<_t>(), nullptr);
		}

		_t temp = _t{args...}; // this gets moved
		return add(handle, make_component<_t>(), &temp);
	}

	template<typename _t>
	entity remove(entity_handle handle)
	{
		return remove(handle, make_component<_t>());
	}

	template<typename _t>
	_t& get(entity_handle handle)
	{
		_t* ptr;
		get(handle, make_component<_t>(), (void**)&ptr);
		return *ptr;
	}

	template<typename _t>
	bool has(entity_handle handle)
	{
		return is_in_archetype(handle, make_component<_t>());
	}

	template<typename... _q>
	entity_query<false, _q...> query()
	{
		entity_query<false, _q...> result(this);

		std::array<hash_t, sizeof...(_q)> query = make_query<_q...>();
		for (auto& [_, storage] : m_storage)
		{
			if (is_query_match<sizeof...(_q)>(storage.m_archetype, query))
			{
				result.m_matches.push_back(&storage);
			}
		}

		return result;
	}

private:
	void clean_storage(const entity_storage& storage)
	{
		// this breaks entity bc storage pointer is invalid
		// should think more about it, for now just keep empty storages
		return;
		if (storage.m_count == 0)
		{
			m_storage.erase(storage.m_archetype.m_hash);
		}
	}

	// helpers to listen for events
	// can only remove all at once, but I think that's fine
public:
	void on_destroy(entity_handle handle, const std::function<void(entity)>                  & func) { add_listener<false>(handle, entity_command::DESTROY, func); }
	void on_set    (entity_handle handle, const std::function<void(entity)>                  & func) { add_listener<false>(handle, entity_command::SET,     func); }
	void on_set    (entity_handle handle, const std::function<void(entity, const component&)>& func) { add_listener<true> (handle, entity_command::SET,     func); }
	void on_add    (entity_handle handle, const std::function<void(entity)>                  & func) { add_listener<false>(handle, entity_command::ADD,     func); }
	void on_add    (entity_handle handle, const std::function<void(entity, const component&)>& func) { add_listener<true> (handle, entity_command::ADD,     func); }
	void on_remove (entity_handle handle, const std::function<void(entity)>                  & func) { add_listener<false>(handle, entity_command::REMOVE,  func); }
	void on_remove (entity_handle handle, const std::function<void(entity, const component&)>& func) { add_listener<true> (handle, entity_command::REMOVE,  func); }

private:
	template<bool _include_component>
	using callback_func = typename std::conditional<
		_include_component,
		std::function<void(entity, const component&)>,
		std::function<void(entity)>>::type;

	struct callback_base
	{
		entity_command m_command;
		virtual ~callback_base() = default;
		virtual void call(entity entity, const component* component) = 0;
	};

	template<bool _include_component>
	struct callback : callback_base
	{
		callback_func<_include_component> m_callback;
		void call(entity entity, const component* component) override
		{
			if constexpr (_include_component) m_callback(entity, *component);
			else                              m_callback(entity);
		}
	};

	std::unordered_map<hash_t, std::vector<callback_base*>> m_callbacks;

	void call_listener(entity_command command, entity_handle handle, const component* component)
	{
		auto itr = m_callbacks.find(handle.hash());
		if (itr != m_callbacks.end())
		{
			for (callback_base* callback : itr->second)
			{
				if (callback->m_command == command)
				{
					callback->call(wrap(handle), component);
				}
			}
		}
	}

	template<bool _include_component>
	void add_listener(entity_handle handle, entity_command command, const callback_func<_include_component>& func)
	{
		callback<_include_component>* c = new callback<_include_component>(); // see below //
		c->m_command = command;
		c->m_callback = func;

		m_callbacks[handle.hash()].push_back(c);
	}

	void remove_listeners(entity_handle handle)
	{
		auto itr = m_callbacks.find(handle.hash());
		if (itr != m_callbacks.end())
		{
			for (callback_base* callback : itr->second)
			{
				delete callback;								    // see above //
			}

			m_callbacks.erase(itr);
		}
	}
};

// defer actions until a point when they can all be executed at once
// for example deleting entities inside a query loop
// not thread safe, but should be

struct command_buffer
{
	struct command
	{
		entity_command m_type;
		const char* m_where;
		virtual ~command() = default;
		virtual void execute(entity_manager* manager) = 0;
	};

	// could this get replaced with std::bind?

	struct command_create : command
	{
		archetype m_archetype;
		void execute(entity_manager* manager) { manager->create(m_archetype); }
	};

	struct command_destroy : command
	{
		entity_handle m_handle;
		void execute(entity_manager* manager) { manager->destroy(m_handle); }
	};

	template<typename _t>
	struct command_set : command
	{
		entity_handle m_handle;
		component m_component;
		_t m_data;
		void execute(entity_manager* manager) { manager->set(m_handle, m_component, &m_data); }
	};

	template<typename _t>
	struct command_add : command
	{
		entity_handle m_handle;
		component m_component;
		_t m_data;
		void execute(entity_manager* manager) { manager->add(m_handle, m_component, &m_data); }
	};

	struct command_remove : command
	{
		entity_handle m_handle;
		component m_component;
		void execute(entity_manager* manager) { manager->remove(m_handle, m_component); }
	};

	std::vector<command*> m_queue;
	entity_manager* m_manager;
	const char* m_where_current; // name of current location, useful for debugging

	command_buffer(
		entity_manager* manager
	)
		: m_manager       (manager)
		, m_where_current (nullptr)
	{}

	void create(const archetype& archetype)
	{
		command_create* command = queue<command_create>();
		command->m_type = entity_command::CREATE;
		command->m_archetype = archetype;
	}

	void destroy(entity entity)
	{
#ifdef IW_ENTITY_DEBUG
		for (command_buffer::command* c : m_queue)
		{
			if (c->m_type == entity_command::DESTROY)
			{
				command_destroy* cd = (command_destroy*)c;

				if (cd->m_handle.m_entity == entity.m_handle.m_entity)
				{
					printf("\n[Entity] Tried to double queue a delete");
					printf("\n[Entity]\t 1. Index: %d from %s", cd->   m_handle.m_entity, cd->m_where);
					printf("\n[Entity]\t 2. Index: %d from %s", entity.m_handle.m_entity, m_where_current);
					//__debugbreak();
					return;
				}
			}
		}

		if (!entity.is_alive())
		{
			printf("\n[Entity] Tried queue a delete with a dead entity");
			printf("\n[Entity]\t 1. Index: %d from %s", entity.m_handle.m_entity, m_where_current);
		}
#endif
		command_destroy* command = queue<command_destroy>();
		command->m_type = entity_command::DESTROY;
		command->m_handle = entity.m_handle;
	}

	void set(entity entity, const component& component) // no data
	{
		command_set<void*>* command = queue<command_set<void*>>();
		command->m_type = entity_command::SET;
		command->m_handle = entity.m_handle;
		command->m_component = component;
		command->m_data = nullptr;
	}

	void add(entity entity, const component& component)  // no data
	{
		command_add<void*>* command = queue<command_add<void*>>();
		command->m_type = entity_command::ADD;
		command->m_handle = entity.m_handle;
		command->m_component = component;
		command->m_data = nullptr;
	}

	void remove(entity entity, const component& component)
	{
		command_remove* command = queue<command_remove>();
		command->m_type = entity_command::REMOVE;
		command->m_handle = entity.m_handle;
		command->m_component = component;
	}

	// template helpers

	template<typename _t, typename... _args>
	void set(entity entity, _args&&... args)
	{
		command_set<_t>* command = queue<command_set<_t>>();
		command->m_type = entity_command::SET;
		command->m_handle = entity.m_handle;
		command->m_component = make_component<_t>();
		command->m_data = _t(args...);
	}

	template<typename _t, typename... _args>
	void add(entity entity, _args&&... args)
	{
		command_add<_t>* command = queue<command_add<_t>>();
		command->m_type = entity_command::ADD;
		command->m_handle = entity.m_handle;
		command->m_component = make_component<_t>();
		command->m_data = _t(args...);
	}

	template<typename _t>
	void remove(entity entity)
	{
		command_remove* command = queue<command_remove>();
		command->m_type = entity_command::REMOVE;
		command->m_handle = entity.m_handle;
		command->m_component = make_component<_t>();
	}

	void execute()
	{
		for (command* command : m_queue)
		{
			command->execute(m_manager);
			delete command;
		}

		m_queue.clear();
	}
private:
	template<typename _t>
	_t* queue()
	{
		static_assert(std::is_base_of<command, _t>::value, "_t must inherit from command");

		_t* t = new _t();
		m_queue.emplace_back(t);
		t->m_where = m_where_current;
		return t;
	}
};

inline
void entity::destroy()
{
	assert(m_manager && "entity::destroy failed, no manager");
	m_manager->destroy(m_handle);
	m_handle = {};
	m_store = nullptr;
	m_manager = nullptr;
}

template<typename _t, typename... _args>
entity& entity::set(_args&&... args)
{
	// cant use store because of events

	assert(m_manager && "entity::set failed, no manager");
	m_manager->set<_t>(m_handle, std::forward<_args>(args)...);
	return *this;
}

template<typename _t, typename... _args>
entity& entity::add(_args&&... args)
{
	assert(m_manager && "entity::add failed, no manager");
	*this = m_manager->add<_t>(m_handle, std::forward<_args>(args)...);
	return *this;
}

template<typename _t>
entity& entity::remove()
{
	assert(m_manager && "entity::remove failed, no manager");
	*this = m_manager->remove<_t>(m_handle);
	return *this;
}

inline entity& entity::on_destroy(const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_destroy failed, no manager"); m_manager->on_destroy(m_handle, func); return *this; }
inline entity& entity::on_set    (const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_add failed, no manager");     m_manager->on_set    (m_handle, func); return *this; }
inline entity& entity::on_set    (const std::function<void(entity, const component&)>& func) { assert(m_manager && "entity::on_add failed, no manager");     m_manager->on_set    (m_handle, func); return *this; }
inline entity& entity::on_add    (const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_set failed, no manager");     m_manager->on_add    (m_handle, func); return *this; }
inline entity& entity::on_add    (const std::function<void(entity, const component&)>& func) { assert(m_manager && "entity::on_set failed, no manager");     m_manager->on_add    (m_handle, func); return *this; }
inline entity& entity::on_remove (const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_remove failed, no manager");  m_manager->on_remove (m_handle, func); return *this; }
inline entity& entity::on_remove (const std::function<void(entity, const component&)>& func) { assert(m_manager && "entity::on_remove failed, no manager");  m_manager->on_remove (m_handle, func); return *this; }

/*
	If you want to use singletons, note dll bounds
*/
#define IW_USE_SINGLETONS

/*
	If you want to log the location of the dispatcher
*/
#define IW_LOG_DEFER

#ifdef IW_USE_SINGLETONS
	inline
	entity_manager& entities()
	{
		static entity_manager manager;
		return manager;
	}

	inline
	command_buffer& entities_defer(const char* where_from = nullptr) // should disable this arg if no LOG_DEFER
	{
		static command_buffer buffer(&entities());
		buffer.m_where_current = where_from;
		return buffer;
	}

#	ifdef IW_LOG_DEFER
#		include "Macros.h"
#		define entities_defer() entities_defer(__LOCATION)
#	endif
#endif
