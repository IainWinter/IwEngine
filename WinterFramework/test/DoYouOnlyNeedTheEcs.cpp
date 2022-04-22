#include "../Entity.h"
#include "../ext/serial_common.h"
#include "../ext/multi_vector.h"
#include "../ext/serial_json.h"
#include <iostream>

struct window
{
	
};

struct vec3
{
	float x, y, z;
};

struct rotor
{
	float x, y, z, w;
};

struct transform
{
	vec3 position;
	vec3 scale;
	rotor rotation; 
};

struct enemy
{
	entity target;
	float shots_per_second;
	int health;
};

struct static_collider
{
	
};

struct static_body
{
	static_collider* collider;
};

struct static_physics
{

};

struct dynamic_physics
{

};

struct pos {};
struct vel {};

struct other {};

struct meta_entity
{
	int m_index;
	int m_version;
	meta::multi_vector m_components;
};

struct meta_entity_store
{
	hash_t m_archetype;
	std::vector<std::string> m_components;
	std::vector<meta_entity> m_entities;
};

namespace meta
{
	template<>
	void serial_write(serial_writer* serial, const entity& value)
	{
		serial->write(value.m_handle);
	}

	template<>
	void serial_read(serial_reader* serial, entity& value)
	{
		serial->read(value.m_handle);
	}
}

int main()
{
	meta::describe<vec3>()
		.name("vec3")
		.member<&vec3::x>("x")
		.member<&vec3::y>("y")
		.member<&vec3::z>("z");

	meta::describe<rotor>()
		.name("rotor")
		.member<&rotor::x>("x")
		.member<&rotor::y>("y")
		.member<&rotor::z>("z")
		.member<&rotor::w>("w");

	meta::describe<transform>()
		.name("transform")
		.member<&transform::position>("position")
		.member<&transform::scale>("scale")
		.member<&transform::rotation>("rotation");

	meta::describe<enemy>()
		.name("enemy")
		.member<&enemy::target>("target")
		.member<&enemy::shots_per_second>("shots_per_second")
		.member<&enemy::health>("health");

	meta::describe<entity_handle>()
		.name("entity handle")
		.member<&entity_handle::m_index>("index")
		.member<&entity_handle::m_version>("version")
		.member<&entity_handle::m_archetype>("archetype");

	meta::describe<meta_entity>()
		.name("entity")
		.member<&meta_entity::m_index>("index")
		.member<&meta_entity::m_version>("version")
		.member<&meta_entity::m_components>("components");

	meta::describe<meta_entity_store>()
		.name("entity store")
		.member<&meta_entity_store::m_archetype>("archetype")
		.member<&meta_entity_store::m_components>("components")
		.member<&meta_entity_store::m_entities>("entities");

	entity application = entities().create<window, static_physics, dynamic_physics>();
	entities().create<transform, enemy>();
	entities().create<transform, enemy>();
	entities().create<transform, enemy>();

	std::stringstream ss;

	json_writer json(ss);

	std::vector<meta_entity_store> these;

	for (const auto& [_, store] : entities().m_storage)
	{
		meta_entity_store& meta_store = these.emplace_back();

		meta_store.m_archetype = store.m_archetype.m_hash;
		for(const component& c : store.m_archetype.m_components)
		{
			meta_store.m_components.push_back(c.m_info->m_name);
		}

		for (pool_iterator itr(store.m_pool); itr.more(); itr.next())
		{
			entity_data* data = itr.get<entity_data>();

			meta_entity& ent = meta_store.m_entities.emplace_back();
			ent.m_index = data->m_index;
			ent.m_version = data->m_version;

			for (int i = 0; i < store.m_archetype.m_components.size(); i++)
			{
				ent.m_components.push(
					store.m_archetype.m_components.at(i).m_type,
					(char*)data->m_components + store.m_archetype.m_offsets.at(i));
			}
		}
	}

	json.write(these);

	std::istringstream iss(ss.str());

	
}
