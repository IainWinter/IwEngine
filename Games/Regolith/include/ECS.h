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

struct component
{
	hash_t m_hash; // low 16 bits are the repeat count
	int m_size;
	std::function<void(void*)>        m_destructor;
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
		c.m_destructor = [](void* ptr) { ((_t*)ptr)->~_t(); };
		c.m_move = [](void* ptr, void* data) { new (ptr) _t(std::move(*(_t*)data)); };
	}
	return c;
}

struct archetype
{
	hash_t m_hash;
	int m_size;
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
			return a.m_hash < b.m_hash;
		}
	);

	std::unordered_map<hash_t, rep_t> counts;
	for (component& c : a.m_components)
	{
		set_repeats(c.m_hash, counts[just_hash(c.m_hash)]++);

		a.m_offsets.push_back(a.m_size);
		a.m_size += c.m_size;
		a.m_hash ^= (c.m_hash + 0x9e3779b9) + (a.m_hash << 6);
	}

	return a;
}

inline
archetype add_to_archetype(
	const archetype& archetype,
	const std::vector<component>& new_components)
{
	std::vector<component> components = archetype.m_components;
	components.insert(components.end(), new_components.begin(), new_components.end());
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

		if (just_hash(c.m_hash) == component.m_hash)
		{
			return archetype.m_offsets.at(i) + c.m_size * get_repeats(c.m_hash);
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
	bool matches = true;
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
			matches = false;
			break;
		}
	}

	return matches;
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

struct entity_handle
{
	int    m_index;
	int    m_version;
	hash_t m_archetype;

	operator hash_t() // for ==
	{
		hash();
	}

	hash_t hash() const
	{
		return m_index ^ 17 * m_version ^ 31 * m_archetype;
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
		entity_handle handle;
		handle.m_archetype = m_archetype.m_hash;

		entity_data* data;

		if (m_recycle != -1)
		{
			handle.m_index = m_recycle;
			data = &m_entities.at(handle.m_index);

			m_recycle = (int)data->m_addr;
		}

		else
		{
			handle.m_index = m_entities.size();
			data = &m_entities.emplace_back();
			data->m_version = version;
		}

		handle.m_version = data->m_version;
		data->m_addr = m_pool.alloc(m_archetype.m_size);
		data->m_index = handle.m_index;

		m_count += 1;

		return handle;
	}

	void destroy_entity(
		entity_handle handle,
		bool destroy_components = true)
	{
		assert(handle.m_archetype == m_archetype.m_hash);
		assert(handle.m_version   == m_entities.at(handle.m_index).m_version);

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
		return handle.m_version == m_entities.at(handle.m_index).m_version;
	}

	entity_handle move_entity(
		entity_handle old_entity,
		entity_storage& new_store)
	{
		entity_handle new_entity = new_store.create_entity(old_entity.m_version + 1);

		void* old_data =           m_entities.at(old_entity.m_index).m_addr;
		void* new_data = new_store.m_entities.at(new_entity.m_index).m_addr;

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
					offset_raw_pointer(old_data, old_component),
					offset_raw_pointer(new_data, new_component)
				);
			}
		}

		destroy_entity(old_entity, false);
		return new_entity;
	}

	void set_component(
		entity_handle handle,
		const component& component,
		void* data)
	{
		component.m_move(get_raw_pointer(handle, component), data);
	}

	void* get_raw_pointer(
		entity_handle handle,
		const component& component)
	{
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
};

struct entity_manager; // needed for destroy, add

struct entity
{
	entity_handle   m_handle;
	entity_storage* m_store;
	entity_manager* m_manager;

	void destroy(); // defined under entity_manager

	bool is_alive()
	{
		return m_store->is_entity_alive(m_handle);
	}

	operator hash_t()
	{
		return m_handle.hash();
	}

	template<typename _t, typename... _args>
	entity& set(_args&&... args)
	{
		m_store->set_component(m_handle, make_component<_t>(), &_t(args...));
		return *this;
	}

	template<typename _t, typename... _args>
	entity& add(_args&&... args)
	{
		*this = m_manager->add(m_handle, make_component<_t>(), &_t(args...));
		return *this;
	}

	template<typename _t>
	_t& get()
	{
		_t* ptr;
		m_store->set_component(m_handle, make_component<_t>(), (void**)&ptr);
		return *ptr;
	}

	template<typename _t>
	bool has()
	{
		return is_in_archetype(m_store->m_archetype, make_component<_t>());
	}
};

// should make q need an entity in there

template<bool _with_entity, typename... _q>
struct entity_query
{
	struct itr
	{
		std::vector<entity_storage::itr>            m_curs;
		std::vector<entity_storage::itr>            m_ends;
		std::vector<std::array<int, sizeof...(_q)>> m_offsets;
		std::vector<entity_storage*>                m_stores;

		int m_current; // current item

		itr(
			std::vector<entity_storage*> matches,
			bool end
		)
			: m_current (end ? matches.size() : 0)
			, m_stores  (matches)
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

		typename std::conditional<_with_entity,
			std::tuple<entity, _q&...>,
			std::tuple<        _q&...>>::type operator*() const
		{
			if constexpr (_with_entity)
			{
				const entity_storage::entity_data& cur = *m_curs.at(m_current);
				entity_storage* store = m_stores.at(m_current);

				entity_handle handle = { cur.m_index, cur.m_version, store->m_archetype.m_hash };

				return std::tuple_cat(
					std::make_tuple<entity>({ handle, store }),
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

	int count() const
	{
		int c = 0;
		for (entity_storage* store : m_matches) c += store->m_count;
		return c;
	}

	itr begin() { return itr(m_matches, false); }
	itr end()   { return itr(m_matches, true);  }

	entity_query<true>&        only_entity() { return *(entity_query<true       >*)this; }
	entity_query<true, _q...>& with_entity() { return *(entity_query<true, _q...>*)this; }

	template<typename... _o>
	entity_query<_with_entity, _o...>& only() { return *(entity_query<_with_entity, _o...>*)this; }

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
};

struct entity_manager
{
	std::unordered_map<hash_t, entity_storage> m_storage;
	std::unordered_map<hash_t, entity_handle*> m_listen_for_move;

	entity create(const archetype& archetype)
	{
		entity_handle handle = get_storage(archetype).create_entity();
		return wrap(handle);
	}

	void destroy(entity_handle handle)
	{
		entity_storage& store = m_storage.at(handle.m_archetype);
		store.destroy_entity(handle);
		clean_storage(store);
	}

	bool is_alive(entity_handle handle)
	{
		m_storage.at(handle.m_archetype).is_entity_alive(handle);
	}

	void set(entity_handle handle, const component& component, void* data)
	{
		m_storage.at(handle.m_archetype).set_component(handle, component, data);
	}

	entity add(entity_handle handle, const component& component, void* data = nullptr)
	{
		entity_storage& old_store = m_storage.at(handle.m_archetype);
		entity_storage& new_store = get_storage(add_to_archetype(old_store.m_archetype, { component }));

		entity_handle new_handle = old_store.move_entity(handle, new_store);

		if (data)
		{
			new_store.set_component(new_handle, component, data);
		}

		auto itr = m_listen_for_move.find(handle.hash());
		if (itr != m_listen_for_move.end())
		{
			*itr->second = new_handle;
			m_listen_for_move.emplace(new_handle.hash(), itr->second);
			m_listen_for_move.erase(itr);
		}

		clean_storage(old_store);

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

	entity_storage& get_storage(const archetype& archetype)
	{
		auto itr = m_storage.find(archetype.m_hash);
		if (itr == m_storage.end())
		{
			itr = m_storage.emplace_hint(itr, archetype.m_hash, entity_storage(archetype));
		}

		return itr->second;
	}

	// helpers

	void listen_for_move(entity_handle* entity_location)
	{
		m_listen_for_move.emplace(entity_location->hash(), entity_location);
	}

	void stop_listen_for_move(entity_handle handle)
	{
		auto itr = m_listen_for_move.find(handle.hash());
		if (itr != m_listen_for_move.end())
		{
			m_listen_for_move.erase(itr);
		}
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
		entity_query<false, _q...> result;

		std::array<hash_t, sizeof...(_q)> query = make_query<_q...>();
		for (auto& [hash, storage] : m_storage)
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
		if (storage.m_count == 0)
		{
			m_storage.erase(storage.m_archetype.m_hash);
		}
	}
};


entity_manager& entities();

inline void entity::destroy()
{
	m_manager->destroy(m_handle);
	m_handle = {};
	m_store = nullptr;
	m_manager = nullptr;
}
