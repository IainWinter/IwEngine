#pragma once

#include "entt/entity/registry.hpp"
#include <type_traits>

template<typename... _t>
using tuple_or_single = typename std::conditional<
							sizeof...(_t) == 1u,
							typename std::tuple_element<0, std::tuple<_t&...>>::type,
							typename std::tuple<_t&...>
						>::type;

struct entity
{
	entt::entity m_handle;
	entt::registry* m_store;

	entity() : m_handle(), m_store(nullptr) {}

	entity(
		entt::entity handle,
		entt::registry* store
	)
		: m_handle (handle)
		, m_store  (store)
	{}

	bool operator==(const entity& other) const { return m_handle == other.m_handle; }
	bool operator!=(const entity& other) const { return !operator==(other); }

	void destroy()
	{
		m_store->destroy(m_handle);
	}

	bool is_alive() const
	{
		return m_store && m_store->valid(m_handle);
	}

	template<typename... _t>
	tuple_or_single<_t...> get()
	{
		return m_store->get<_t...>(m_handle);
	}

	template<typename... _t>
	bool has() const
	{
		return m_store->all_of<_t...>(m_handle);
	}

	template<typename _t, typename... _args>
	entity& set(_args&&... args)
	{
		m_store->replace<_t>(m_handle, std::forward<_args>(args)...); // this causes assertion
		return *this;
	}

	template<typename _t, typename... _args>
	entity& add(_args&&... args)
	{
		m_store->emplace<_t>(m_handle, std::forward<_args>(args)...);
		return *this;
	}

	template<typename... _t>
	entity& remove()
	{
		m_store->remove<_t...>(m_handle);
		return *this;
	}
};

struct entity_manager
{
	entt::registry m_store;

	entity create()
	{
		return entity(m_store.create(), &m_store);
	}

	template<typename... _t>
	auto query()
	{
		// if constexpr (sizeof...(_t) > 1)
		// {
		// 	return m_store.group<_t...>().each();
		// }

		return m_store.view<_t...>().each();
	}

	template<typename... _t>
	tuple_or_single<_t...> first()
	{
		// is there a way to get this better?
		return entity(m_store.view<_t...>().front(), &m_store).get<_t...>();
	}
};

entity_manager& entities()
{
	static entity_manager reg;
	return reg;
}

struct System
{
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void ImGui() {}
};

template<typename... _t>
struct For_System : System
{
	void Update()
	{
		entities().m_store.view<_t...>().each([this](entt::entity e, _t&... components)
		{
			Update(entity(e, &entities().m_store), components...);
		});
	}

	virtual void Update(entity e, _t&... components) = 0;
};

struct Delete {};