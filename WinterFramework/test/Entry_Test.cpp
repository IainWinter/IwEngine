#include "../Entry.h"
#include "../Entity.h"
#include "../SimpleServer.h"

#include <iostream>

#include "../ext/serial_json.h"
#include "../ext/serial_common.h"
#include "../ext/multi_vector.h"

#include "../ext/Components_meta.h"

static asio::io_context s_asio;
static std::thread s_asio_thread;
static server<uint32_t> s_server(s_asio, 15001);

static int s_frame;

enum class editor_request_kind
{
	GET_ENTITY_CHILDREN,
	GET_ENTITY_COMPONENTS
};

struct editor_request
{
	editor_request_kind m_kind;
	std::vector<hash_t> m_data;
};

struct editor_get_entity_components_result
{
	std::vector<std::string> m_names;
	//std::vector<std::string> m_kind;
	meta::multi_vector m_components;

	// void add_type(meta::type* type)
	// {
	// 	if (type->m_info->m_name == "vec3")
	// }
};

struct my_component
{
	vec2 position_2d;	
	vec3 something_in_3d;

	vec4b flags;

	vec2i ints;	
};

void setup()
{
	printf("hello\n");
	printf("message header size is %d bytes\n", (int)sizeof(message_header<uint32_t>));

	meta::describe<editor_request>()
		.name("editor_request")
		.member<&editor_request::m_kind>("kind")
		.member<&editor_request::m_data>("data");

	meta::describe<entity_handle>()
		.name("entity_handle")
		.member<&entity_handle::m_index>("index")
		.member<&entity_handle::m_version>("version")
		.member<&entity_handle::m_archetype>("archetype");

	meta::describe<editor_get_entity_components_result>()
		.name("editor_get_entity_components_result")
		.member<&editor_get_entity_components_result::m_names>("names")
		.member<&editor_get_entity_components_result::m_components>("components");

	meta::describe<my_component>()
		.name("my_component")
		.member<&my_component::position_2d>("position_2d")
		.member<&my_component::something_in_3d>("something_in_3d")
		.member<&my_component::flags>("flags")
		.member<&my_component::ints>("ints");

	register_component_types();

    s_server.async_accept();
	s_asio_thread = std::thread([]() { s_asio.run(); });

	for (int i = 0; i < 10; i++)
	{
		entity e = entities().create<Transform, NameComponent, int, my_component>();
		e.get<NameComponent>().Name = "noice";
		e.get<Transform>().Position.x = rand() / (float)RAND_MAX;
		e.get<Transform>().Position.y = rand() / (float)RAND_MAX;
		e.get<Transform>().Position.z = rand() / (float)RAND_MAX;
	}

	for (int i = 0; i < 10; i++)
	{
		entity e = entities().create<Transform, NameComponent, int, float>();
		e.get<NameComponent>().Name = "noice 2";
	}
}

bool loop()
{
	s_frame += 1;

	owned_message<uint32_t> msg = s_server.m_in.pop_front();
	printf("[%d] incomming %d bytes says: '%s'\n", s_frame, msg.m_header.m_size, msg.m_text.data());

	std::istringstream iss(msg.m_text);
	json_reader reader(iss);

	if (reader.is_valid())
	{
		editor_request request; reader.read(request);

		switch(request.m_kind)
		{
			case editor_request_kind::GET_ENTITY_CHILDREN:
			{
				entity_handle get_my_children;
				get_my_children.m_archetype = request.m_data.at(0);

				if (get_my_children.m_archetype == -1) // means we want to list every entity
				{
					std::vector<entity_handle> handles;

					for (const auto& [_, store] : entities().m_storage)
					for (pool_iterator itr(store.m_pool); itr.more(); itr.next())
					{
						entity_data* data = itr.get<entity_data>();
						handles.push_back(store.wrap(data));
					}

					std::stringstream ss;
					json_writer(ss).write(handles);
					
					msg.m_session->write(make_message(msg.m_header.m_id, ss.str()));
				}

				break;
			}
			case editor_request_kind::GET_ENTITY_COMPONENTS:
			{
				entity_handle get_my_components;
				get_my_components.m_index = request.m_data.at(0);	
				get_my_components.m_version = request.m_data.at(1);	
				get_my_components.m_archetype = request.m_data.at(2);

				// get storage from arch hash

				auto itr = entities().m_storage.find(get_my_components.m_archetype);
				if (itr == entities().m_storage.end())
				{
					msg.m_session->write(make_message(msg.m_header.m_id, "[]"));
					break; // exit if no storage
				}

				entity_storage& store = itr->second;
				editor_get_entity_components_result result;
				entity_data* data = (entity_data*)store.m_pool.get_block_address(get_my_components.m_index);

				for (int i = 0; i < store.m_archetype.m_components.size(); i++)
				{
					const component& c = store.m_archetype.m_components.at(i);

					if (!c.m_type->has_members())
					{
						printf("Component '%s' has no members. skipping...\n", c.m_info->m_name.c_str());
						continue;
					}

					meta::type* type = c.m_type;
					result.m_names.push_back(type->m_info->m_name);
					result.m_components.push(type, (char*)data->m_components + store.m_archetype.m_offsets.at(i));
				}

				std::stringstream ss;
				json_writer(ss).write(result);
				
				msg.m_session->write(make_message(msg.m_header.m_id, ss.str()));

				break;
			}
		}
	}

	bool exit = ( 1 << 15 ) & GetAsyncKeyState(VK_ESCAPE);

	if (exit)
	{
		s_asio.stop();
		if (s_asio_thread.joinable())
		{
			s_asio_thread.join();
		}
	}

	return !exit;
} 