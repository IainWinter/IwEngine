#pragma once

#include "iw/util/set/sparse_set.h"
#include "iw/util/memory/pool_allocator.h"
#include <tuple>
#include <functional>
#include <type_traits>
#include <assert.h>

using hash_t = uint64_t;
using rep_t  = uint16_t; // low bits of the hash to represent repeat components in archetype 

using hash_bits = std::integral_constant<int, ~(hash_t)rep_t(-1) & hash_t(-1)>;
using rep_bits  = std::integral_constant<int,  (hash_t)rep_t(-1)>;

constexpr inline void set_repeats(hash_t& hash, rep_t count)
{
	hash = (hash & hash_bits::value) | (hash_t)count;
}

constexpr inline rep_t get_repeats(const hash_t& hash)
{
	return (rep_t)(hash & rep_bits::value);
}

constexpr inline hash_t just_hash(const hash_t hash)
{
	return hash & hash_bits::value;
}

#ifndef max // Windows.h and other system headers define these

template<typename _t>
constexpr inline _t max(_t a, _t b)
{
	return a > b ? a : b;
}

template<typename _t>
constexpr inline _t min(_t a, _t b)
{
	return a < b ? a : b;
}

#endif

// needed for event_manager::callbacks

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second) * 31;
    }
};

struct component
{
	hash_t m_hash = 0; // low 16 bits are the repeat count
	int m_size    = 0;
	const char* m_name = nullptr;
	std::function<void(void*)>        m_destructor;
	std::function<void(void*)>        m_default;
	std::function<void(void*, void*)> m_move;
};

template<typename _t>       // this should be the only place that templates are necessary for storage
component make_component()  //	everyother template function is short-hand back to this
{					  
	static component c = { 0, 0 }; // dll bounds..., could remove 'static'

	if (c.m_hash == 0)
	{
		c.m_hash = just_hash(typeid(_t).hash_code());
		c.m_size = sizeof(_t);
		c.m_name = typeid(_t).name();
		c.m_destructor = [](void* ptr)             { ((_t*)ptr)->~_t(); };
		c.m_default    = [](void* ptr)             { new (ptr) _t(); };
		c.m_move       = [](void* ptr, void* data) { new (ptr) _t(std::move(*(_t*)data)); };
	}
	return c;
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
		a.m_size += c->m_size;
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
			return archetype.m_offsets.at(i) + c.m_size * get_repeats(component.m_hash);
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

template<typename... _t>
std::vector<component> component_list()
{
	return { make_component<_t>()... };
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

constexpr hash_t hash_int3(int x, int y, hash_t z)
{
	hash_t hash = 0x9e3779b9;
	hash = (hash ^ (hash_t)x) * 0x56a34beefff;
	hash = (hash ^ (hash_t)y) * 0x56ff3eea4;
	hash = (hash ^         z) * 0xfabcdef1;
	return hash;
}

struct entity_handle
{
	int    m_index     = 0;
	int    m_version   = 0;
	hash_t m_archetype = 0;

	bool operator==(const entity_handle& other) const { return hash() == other.hash(); }
	bool operator!=(const entity_handle& other) const { return !operator==(other); }

	hash_t hash() const
	{
		return hash_int3(m_index, m_version, m_archetype);
	}
};

struct entity_storage
{
	struct entity_data
	{
		int m_index;   // for query iterator only
		int m_version; // for if entity coming in is stale
		void* m_addr;
	};

	using store_t = std::vector<entity_data>;
	using itr = store_t::iterator;

	archetype m_archetype;
	iw::pool_allocator m_pool;
	int m_recycle; // index to head of delected entities, m_addr becomes index to next 
	int m_count;   // number of alive entities

	store_t m_entities;

	entity_storage(
		const archetype& archetype
	)
		: m_archetype (archetype)
		, m_pool      (archetype.m_size, 16)
		, m_recycle   (-1)
		, m_count     (0)
	{}

	entity_handle create_entity(
		int version = 0)
	{
		entity_data* data;

		if (m_recycle != -1)
		{
			data = &m_entities.at(m_recycle);
			m_recycle = (int)data->m_addr;
		}

		else
		{
			data = &m_entities.emplace_back();
			data->m_index = m_entities.size() - 1;
			data->m_version = version;
		}

		data->m_addr = m_pool.alloc(m_archetype.m_size);

		for (const component& component : m_archetype.m_components)
		{
			component.m_default(offset_raw_pointer(data->m_addr, component));
		}

		m_count += 1;

		return wrap(*data);
	}

	void destroy_entity(
		entity_handle handle,
		bool destroy_components = true)
	{
		assert(handle.m_archetype == m_archetype.m_hash                    && "entity_storage::destroy_entity failed, archetype mismatch");
		assert(handle.m_version == m_entities.at(handle.m_index).m_version && "entity_storage::destroy_entity failed, version mismatch");

		entity_data& data = m_entities.at(handle.m_index);
		
		if (destroy_components)
		{
			for (const component& component : m_archetype.m_components)
			{
				component.m_destructor(offset_raw_pointer(data.m_addr, component));
			}
		}
		
		m_pool.free(data.m_addr, m_archetype.m_size);

		data.m_addr = (void*)m_recycle; // credit skypjack, this is such a good idea, 0's the ptr aswell!!:)
		data.m_version += 1;

		m_recycle = handle.m_index;
		m_count -= 1;
	}

	bool is_entity_alive(
		entity_handle handle)
	{
		assert(handle.m_archetype == m_archetype.m_hash                    && "entity_storage::is_entity_alive failed, archetype mismatch");
		return handle.m_version == m_entities.at(handle.m_index).m_version;
	}

	std::pair<bool, entity_handle> find_from_component(
		const component& component,
		void* component_addr)
	{
		void* addr = (char*)component_addr - offset_of_component(m_archetype, component);

		for (const entity_data& data : m_entities)
		{
			if (data.m_addr == addr)
			{
				return std::make_pair(true, wrap(data));
			}
		}

		return std::make_pair(false, entity_handle {});
	}

	entity_handle move_entity(
		entity_handle handle,
		entity_storage& new_store)
	{
		assert(handle.m_archetype == m_archetype.m_hash                    && "entity_storage::move_entity failed, archetype mismatch");
		assert(handle.m_version == m_entities.at(handle.m_index).m_version && "entity_storage::move_entity failed, version mismatch");

		entity_handle new_handle = new_store.create_entity(handle.m_version + 1);

		void* old_data =           m_entities.at(    handle.m_index).m_addr;
		void* new_data = new_store.m_entities.at(new_handle.m_index).m_addr;

		int old_length =           m_archetype.m_components.size();
		int new_length = new_store.m_archetype.m_components.size();
		int max_length = max(old_length, new_length);

		for (int i = 0; i < max_length; i++)
		{
			int o = min(i, old_length - 1);
			int n = min(i, new_length - 1);

			const component& old_component =           m_archetype.m_components.at(o);
			const component& new_component = new_store.m_archetype.m_components.at(n);

			if (old_component.m_hash == new_component.m_hash)
			{
				old_component.m_move(
					offset_raw_pointer(new_data, new_component),
					offset_raw_pointer(old_data, old_component)
				);
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
		assert(handle.m_archetype == m_archetype.m_hash                    && "entity_storage::set_component failed, archetype mismatch");
		assert(handle.m_version == m_entities.at(handle.m_index).m_version && "entity_storage::set_component failed, version mismatch");

		if (data)
		{
			component.m_move(get_raw_pointer(handle, component), data);
		}
	}

	void* get_raw_pointer(
		entity_handle handle,
		const component& component)
	{
		assert(handle.m_archetype == m_archetype.m_hash                    && "entity_storage::get_raw_pointer failed, archetype mismatch");
		assert(handle.m_version == m_entities.at(handle.m_index).m_version && "entity_storage::get_raw_pointer failed, version mismatch");

		return offset_raw_pointer(m_entities.at(handle.m_index).m_addr, component);
	}

	void* offset_raw_pointer(
		void* ptr,
		const component& component)
	{
		return (char*)ptr + offset_of_component(m_archetype, component);
	}

	itr begin() { return m_entities.begin(); }
	itr end()   { return m_entities.end();   }

private:
	entity_handle wrap(const entity_data& data)
	{
		return { data.m_index, data.m_version, m_archetype.m_hash };
	}
};

struct entity_manager; // needed for destroy / add. Breaks flow a little bit :(
				   // could just reutrn entity_handles, but that was annoying in previous versions

struct entity
{
	entity_handle   m_handle;
	entity_storage* m_store   = nullptr;
	entity_manager* m_manager = nullptr;

	bool operator==(const entity& other) const { return m_handle.operator==(other.m_handle); }
	bool operator!=(const entity& other) const { return m_handle.operator!=(other.m_handle); }
	hash_t hash() const { return m_handle.hash(); }

	void destroy(); // defined under entity_manager

	bool is_alive() const
	{
		if (hash() == hash_int3(0, 0, 0))
		{
			return false;
		}

		assert(m_store && "entity::is_alive failed, no store"); // not needed because above
		return m_store->is_entity_alive(m_handle);
	}

	template<typename _t, typename... _args>
	entity& set(_args&&... args)
	{
		// cant use store because of events

		assert(m_store && "entity::set failed, no manager");
		m_manager->set<_t>(m_handle, std::forward<_args>(args)...);
		return *this;
	}

	template<typename _t, typename... _args>
	entity& add(_args&&... args)
	{
		assert(m_manager && "entity::add failed, no manager");
		*this = m_manager->add<_t>(m_handle, std::forward<_args>(args)...);
		return *this;
	}

	template<typename _t>
	entity& remove()
	{
		assert(m_manager && "entity::remove failed, no manager");
		*this = m_manager->remove<_t>(m_handle);
		return *this;
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
		std::vector<entity_storage::itr>            m_curs;
		std::vector<entity_storage::itr>            m_ends;
		std::vector<std::array<int, sizeof...(_q)>> m_offsets;
		std::vector<entity_storage*>                m_stores;

		int m_current; // current item

		entity_manager* m_manager; // ref to manager, ONLY for returning entity instead of entity_handle

		itr(
			std::vector<entity_storage*> matches,
			entity_manager* manager,
			bool end
		)
			: m_current (end ? matches.size() : 0)
			, m_stores  (matches)
			, m_manager (manager)
		{
			for (entity_storage* storage : matches)
			{
				if (end)
				{
					m_curs.push_back(storage->end());
					// dont need ends or offsets because 
					// checks for m_current block checks into these lists
				}

				else
				{
					m_curs   .push_back(storage->begin());
					m_ends   .push_back(storage->end());
					m_offsets.push_back(query_offsets<_q...>(storage->m_archetype));
				}
			}

			step_to_next_valid();
		}

		std::tuple<_q&...> just_components() const
		{
			return tie_from_offsets<_q...>(m_curs.at(m_current)->m_addr, m_offsets.at(m_current));
		}

		result_t operator*() const
		{
			if constexpr (_with_entity)
			{
				const entity_storage::entity_data& cur = *m_curs.at(m_current);
				entity_storage* store = m_stores.at(m_current);

				entity_handle handle = { cur.m_index, cur.m_version, store->m_archetype.m_hash };

				return std::tuple_cat(
					std::make_tuple<entity>({ handle, store, m_manager }),
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
			do
			{
					 auto& itr = m_curs.at(m_current);
				const auto& end = m_ends.at(m_current);

				++itr;
				if (itr == end) ++m_current;
			} 
			while (is_itr_invalid());

			return *this;
		}

		bool operator==(const itr& other) const 
		{
			return m_current            == other.m_current
				&& (   m_current >= m_curs.size()	      // hack to not overshoot the vectors, fix would be to not use vectors
					|| m_curs.at(m_current) == other.m_curs.at(m_current));
		}

		bool operator!=(const itr& other) const
		{
			return !operator==(other);
		}

		// little bit of a hack

		bool is_itr_invalid() const
		{
			return m_current < m_curs.size() && (long long)m_curs.at(m_current)->m_addr < 1000;
		}

		void step_to_next_valid()
		{
			if (is_itr_invalid()) operator++();
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

	itr begin() { return itr(m_matches, m_manager, false); }
	itr end()   { return itr(m_matches, m_manager, true);  }

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
		m_storage.at(handle.m_archetype).is_entity_alive(handle);
	}

	std::pair<bool, entity> find(const component& component, void* ptr_to_component)
	{
		for (auto& [_, store] : m_storage) // could use query if it didnt need types... (ezish fix)
		{
			if (is_in_archetype(store.m_archetype, component))
			{
				auto [found, handle] = store.find_from_component(component, ptr_to_component);

				if (found)
				{
					return std::make_pair(true, wrap(handle));
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
		set(handle, make_component<_t>(), &_t(args...));
	}

	template<typename _t, typename... _args>
	entity add(entity_handle handle, _args&&... args)
	{
		if constexpr (sizeof...(_args) == 0) {
			return add(handle, make_component<_t>(), nullptr);
		}
		return add(handle, make_component<_t>(), &_t(args...));
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
			if (is_query_match(storage.m_archetype, query))
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
			else						    m_callback(entity);
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
				delete callback;										  // see above //
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
		for (command_buffer::command* c : m_queue)
		{
			if (c->m_type == entity_command::DESTROY)
			{
				if (((command_destroy*)c)->m_handle == entity.m_handle)
				{
					__debugbreak();
					return;
				}
			}
		}

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
		static_assert(std::is_base_of<command, _t>::value);

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

inline entity& entity::on_destroy(const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_destroy failed, no manager"); m_manager->on_destroy(m_handle, func); return *this; }
inline entity& entity::on_set    (const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_add failed, no manager");     m_manager->on_set    (m_handle, func); return *this; }
inline entity& entity::on_set    (const std::function<void(entity, const component&)>& func) { assert(m_manager && "entity::on_add failed, no manager");     m_manager->on_set    (m_handle, func); return *this; }
inline entity& entity::on_add    (const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_set failed, no manager");     m_manager->on_add    (m_handle, func); return *this; }
inline entity& entity::on_add    (const std::function<void(entity, const component&)>& func) { assert(m_manager && "entity::on_set failed, no manager");     m_manager->on_add    (m_handle, func); return *this; }
inline entity& entity::on_remove (const std::function<void(entity)>                  & func) { assert(m_manager && "entity::on_remove failed, no manager");  m_manager->on_remove (m_handle, func); return *this; }
inline entity& entity::on_remove (const std::function<void(entity, const component&)>& func) { assert(m_manager && "entity::on_remove failed, no manager");  m_manager->on_remove (m_handle, func); return *this; }

entity_manager& entities();
command_buffer& entities_defer(const char* where_from = nullptr);

#define LOG_DEFER

#ifdef LOG_DEFER
#	define iw_line_as_string(x) iw_as_string(x)
#	define iw_as_string(x) #x

#	define entities_defer() entities_defer(__FUNCTION__ " " iw_line_as_string(__LINE__))
#endif
