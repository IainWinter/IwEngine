#include "iw/util/set/sparse_set.h"
#include "iw/util/memory/pool_allocator.h"
#include <iostream>

struct component
{
	int m_hash;
	int m_size;
	std::function<void(void*)>        m_destructor;
	std::function<void(void*, void*)> m_move;
};

struct archetype
{
	int m_hash;
	int m_size;
	std::vector<component> m_components;
	std::vector<int> m_offsets;
};

template<typename _t>
component make_component() // this should be the only place that templates are used for storage
{
	static component c = { 0, 0 }; // dll bounds..., could remove 'static'

	if (c.m_hash == 0)
	{
		c.m_hash = typeid(_t).hash_code();
		c.m_size = sizeof(_t);
		c.m_destructor = [](void* ptr) { ((_t*)ptr)->~_t(); };
		c.m_move = [](void* ptr, void* data) { new (ptr) _t(std::move(*(_t*)data)); };
	}
	return c;
}

template<typename... _t>
archetype make_archetype() // doesnt need template, should call a non templated fnuction
{
	return make_archetype({ make_component<_t>()... });
}

archetype make_archetype(const std::vector<component>& components)
{
	archetype a = { 0, 0 };
	a.m_components = components;
	a.m_offsets.reserve(a.m_components.size());
	for (const component& c : a.m_components)
	{
		a.m_offsets.push_back(a.m_size);
		a.m_size += c.m_size;
		a.m_hash ^= c.m_hash + 0x9e3779b9 + a.m_hash << 6;
	}

	return a;
}

template<typename _t>
archetype add_to_archetype(
	const archetype& archetype_)
{
	std::vector<component> components = archetype_.m_components;
	component.push_back(make_component<_t>());
	return make_archetype(components);
}

template<typename _t>
archetype add_to_archetype(
	const archetype& archetype_)
{
	std::vector<component> components = archetype_.m_components;
	component.push_back(make_component<_t>());
	return make_archetype(components);
}

int offset_of_component(const archetype& archetype, const component& component)
{
	for (int i = 0; i < archetype.m_components.size(); i++)
	{
		if (archetype.m_components.at(i).m_hash == component.m_hash)
		{
			return archetype.m_offsets.at(i);
		}
	}

	return -1;
}

template<typename... _q>
std::array<int, sizeof...(_q)> make_query()
{
	return { make_component<_q>().m_hash ... };
}

template<typename... _q>
std::array<int, sizeof...(_q)> query_offsets(
	const archetype& archetype)
{
	return { offset_of_component(archetype, make_component<_q>())... };
}

template<int _s>
bool query_matches(const archetype& archetype, const std::array<int, _s>& hashes)
{
	bool matches = true;
	for (int hash : hashes)
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

struct entity
{
	int m_handle;
	int m_version;
	int m_archetype;

	operator int() // for comparasons
	{
		return m_handle ^ 17 * m_version ^ 31 * m_archetype;
	}
};

struct entity_storage
{
	struct entity_data
	{
		int m_version;
		void* m_addr;
	};

	using itr = iw::sparse_set<int, entity_data>::iterator;

	archetype m_archetype;
	iw::pool_allocator m_pool;
	int m_recycle;              // index to head of delected entities, m_addr becomes index to next 

	iw::sparse_set<int, entity_data> m_entities; // entity idx -> pool alloc addresses

	entity_storage(
		const archetype& archetype
	)
		: m_archetype (archetype)
		, m_pool      (1024)
		, m_recycle   (-1)
	{}

	entity create_entity()
	{
		entity entity;
		entity.m_archetype = m_archetype.m_hash;

		entity_data* data;

		if (m_recycle != -1)
		{
			entity.m_handle = m_recycle;
			data = &m_entities.at(entity.m_handle);

			m_recycle = (int)data->m_addr;
		}

		else
		{
			entity.m_handle = m_entities.size();
			data = &m_entities.emplace(entity.m_handle);
			data->m_version = 0;
		}

		entity.m_version = data->m_version;
		data->m_addr = m_pool.alloc(m_archetype.m_size);

		return entity;
	}

	void destroy_entity(int handle)
	{
		entity_data& data = m_entities.at(handle);
		m_pool.free(data.m_addr, m_archetype.m_size);

		data.m_addr = (void*)m_recycle;
		data.m_version += 1;

		m_recycle = handle;
	}

	void set_component(entity entity, const component& component, void* data)
	{
		component.m_move(get_raw_pointer(entity, component), data);
	}

	void* get_raw_pointer(entity entity, const component& component)
	{
		return (char*)m_entities.at(entity.m_handle).m_addr + offset_of_component(m_archetype, component);
	}

	itr begin() { return m_entities.begin(); }
	itr end()   { return m_entities.end();   }
};

template<typename... _q>
struct entity_query
{
	struct itr
	{
		std::vector<entity_storage::itr>            m_curs;
		std::vector<entity_storage::itr>            m_ends;
		std::vector<archetype*>                     m_archtypes;
		std::vector<std::array<int, sizeof...(_q)>> m_offsets;

		int m_current; // current item

		itr(
			std::vector<entity_storage*> matches,
			bool end
		)
			: m_current (0) 
		{
			for (entity_storage* storage : matches)
			{
				if (end)
				{
					m_current = matches.size();
					m_curs.push_back(storage->end());
				}

				else
				{
					m_curs     .push_back(storage->begin());
					m_ends     .push_back(storage->end());
					m_archtypes.push_back(&storage->m_archetype);
					m_offsets  .push_back(query_offsets<_q...>(storage->m_archetype));
				}
			}
		}

		std::tuple<entity, _q&...> operator*()
		{
			assert(m_archtypes.size() > m_current);

			      auto& cur       = m_curs     .at(m_current);
			const auto& archetype = m_archtypes.at(m_current);
			const auto& offsets   = m_offsets  .at(m_current);

			return std::tuple_cat(
				std::make_tuple<entity>({ cur.sparse_index(), cur->m_version, archetype->m_hash }),
				tie_from_offsets<_q...>(cur->m_addr, offsets)
			);
		}

		itr& operator++()
		{
			      auto& cur = m_curs.at(m_current);
			const auto& end = m_ends.at(m_current);

			if (cur == end) ++m_current;
			else            ++cur;

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
	};

	std::vector<entity_storage*> m_matches;

	itr begin() { return itr(m_matches, false); }
	itr end()   { return itr(m_matches, true);  }
};

struct entity_manager
{
	std::unordered_map<int, entity_storage> m_storage;

	entity create_entity(
		const archetype& a) 
	{
		return m_storage.at(a.m_hash)
						.create_entity();
	}

	template<typename... _t>
	entity create_entity()
	{
		return create_entity(create_archetype<_t...>());
	}

	void destroy_entity(
		entity entity)
	{
		return m_storage.at(entity.m_archetype)
						.destroy_entity(entity.m_handle); 
	}

	template<typename _t>
	void init_component(entity entity, _t&& t)
	{
		m_storage.at(entity.m_archetype)
			.set_component(entity, make_component<_t>(), &_t(std::move(t)));
	}

	template<typename... _q>
	entity_query<_q...> query()
	{
		entity_query<_q...> result;

		std::array<int, sizeof...(_q)> query = make_query<_q...>();
		for (auto& [hash, storage] : m_storage)
		{
			if (query_matches(storage.m_archetype, query))
			{
				result.m_matches.push_back(&storage);
			}
		}

		return result;
	}

	template<typename... _t>
	archetype create_archetype()
	{
		archetype archetype = make_archetype<_t...>();

		if (m_storage.find(archetype.m_hash) == m_storage.end())
		{
			m_storage.emplace(archetype.m_hash, entity_storage(archetype));
		}

		return archetype;
	}
};

#include <chrono>

struct Timer
{
	using clock_t     = std::chrono::high_resolution_clock;
	using res_t       = std::chrono::nanoseconds;
	using duration_t  = std::chrono::duration<double, std::milli>;
	using timepoint_t = std::chrono::time_point<clock_t>;

	timepoint_t m_beginPoint;
	std::string m_name;

	Timer(
		const std::string& name
	)
		: m_beginPoint (clock_t::now())
		, m_name       (name)
	{}

	~Timer()
	{
		timepoint_t endPoint = clock_t::now();

		auto begin = std::chrono::time_point_cast<res_t>(m_beginPoint);
		auto end   = std::chrono::time_point_cast<res_t>(endPoint);

		duration_t duration = end - begin;
		printf("[TIMER] %s took %2.2f", m_name.c_str(), (float)duration.count());
	}
};




#include "App.h"
#include <combaseapi.h>

int App::Initialize(
	iw::InitOptions& options)
{
	SetMenuStateAppVars(APP_VARS_LIST);

	PushLayer<StaticLayer>();
	PushLayer<Audio_Layer>();

	//PushLayer<iw::DebugLayer>();

	int err = Application::Initialize(options);
	if (err) return err;


	// test

	entity_manager manager;
	archetype a = manager.create_archetype<int>();

	for (int i = 1e6; i > 0; i--)
	{
		entity e = manager.create_entity(a);
		manager.init_component<int>(e, rand());
	}

	entity_query<int> ints = manager.query<int>();

	for (auto& [entity, i] : ints)
	{
		printf("%d, %d -> %d\n", entity.m_handle, entity.m_version, i);
	}



	// test





	m_fonts = new iw::FontMap();
	m_fonts->Load("verdana",   18, "fonts/ttf/verdana.ttf");
	m_fonts->Load("verdana",   36, "fonts/ttf/verdana.ttf");
	m_fonts->Load("verdana",   92, "fonts/ttf/verdana.ttf");
	m_fonts->Load("Quicksand", 50, "fonts/ttf/Quicksand-Regular.ttf");
	m_fonts->Load("Quicksand", 24, "fonts/ttf/Quicksand-Regular.ttf");
	m_fonts->Load("Quicksand", 30, "fonts/ttf/Quicksand-Medium.ttf");
	m_fonts->Load("Quicksand", 40, "fonts/ttf/Quicksand-Medium.ttf");
	m_fonts->Load("Quicksand", 60, "fonts/ttf/Quicksand-Medium.ttf");

	PushLayer<iw::ImGuiLayer>(Window, m_fonts)->BindContext();

	Console->QueueCommand("set-state menus");

	// add default cells

	iw::Cell c;
	c.Type        = iw::CellType::LIGHTNING;
	c.Props       = iw::CellProp::NONE;
	c.Style       = iw::CellStyle::SHIMMER;
	c.StyleColor  = iw::Color(.1, .1, .1, 0);
	c.Color       = iw::Color::From255(212, 194, 252);
	c.life        = .1;
	
	iw::Cell::SetDefault(iw::CellType::LIGHTNING, c);

	//m_gamePause = new GameState("Pause menu", StateName::GAME_PAUSE_STATE);
	//m_gamePause->Layers.push_back(new Menu_Pause_Layer());
	//m_gamePause->OnChange = [&]()
	//{
	//	Physics->Paused = true;
	//	//Window()->SetCursor(true);
	//	//Renderer->Device->SetVSync();
	//	Input->SetContext("Menu");
	//};

	//Console->QueueCommand("set-state title");

	// create a guid for the installation of this app

	//GUID install_id;

	//if (iw::FileExists("install_id.txt"))
	//{
	//	std::string str = iw::ReadFile("install_id.txt");
	//	std::wstring s;
	//	s.assign(str.begin(), str.end());

	//	install_id = s;
	//}

	//else
	//{
	//	GUID& id = install_id;
	//	char szGuid[40] = { 0 };
	//	CoCreateGuid(&id);

	//	sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
	//		id.Data1,    id.Data2,    id.Data3, 
	//		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
	//		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

	//	iw::WriteFile("install_id.txt", szGuid);
	//}

	iw::ref<iw::Context> context_game = Input->CreateContext("game");
	context_game->MapButton(iw::InputName::D     , "+right");
	context_game->MapButton(iw::InputName::A     , "-right");
	context_game->MapButton(iw::InputName::W     , "+up");
	context_game->MapButton(iw::InputName::S     , "-up");
	context_game->MapButton(iw::InputName::LMOUSE, "+fire");
	context_game->MapButton(iw::InputName::RMOUSE, "+alt-fire");
	context_game->MapButton(iw::InputName::ESCAPE, "escape");
	context_game->MapButton(iw::InputName::X     , "toolbox");

	context_game->MapButton(iw::InputName::E , "+a"); // buttons for upgrades
	context_game->MapButton(iw::InputName::Q , "+b");
	context_game->MapButton(iw::InputName::F , "+x");
	context_game->MapButton(iw::InputName::C , "+y");

	iw::ref<iw::Context> context_menu = Input->CreateContext("menu");
	context_menu->MapButton(iw::InputName::LMOUSE, "+execute");
	context_menu->MapButton(iw::InputName::ESCAPE, "escape");

	iw::ref<iw::Device> mouse        = Input->CreateDevice<iw::Mouse>();
	iw::ref<iw::Device> keyboard_raw = Input->CreateDevice<iw::RawKeyboard>();

	context_game->AddDevice(mouse);
	context_game->AddDevice(keyboard_raw);

	context_menu->AddDevice(mouse);
	context_menu->AddDevice(keyboard_raw);

	// menu states could be switched to through commands

	// game-play   -- starts game
	// game-end    -- ends game, brings you to post game menus
	// game-exit   -- exits game to main menu
	// game-exit d -- exits game to desktop
	// game-pause  -- opens escape menu

	// todo: all the m_menus-> calls should be handled by the manu layer itself
	//		 all the Input-> calls should be handled in a event listener for state changes

	Console->AddHandler([&](
		const iw::Command& command)
	{
		LOG_INFO << command.Original;

		if (command.Verb == "escape")
		{
			switch (m_state)
			{
				case StateName::IN_MENU:
					m_menus->Escape();
					break;
				case StateName::IN_GAME:
					ChangeToPauseMenu();
					break;
				case StateName::IN_GAME_THEN_MENU:
					if (m_menus->target_menu != MenuTarget::PAUSE)
						m_menus->Escape();
					else
						ChangeBackToGame();
					break;
				default:
					break;
			}
		}

		//else
		//if (command.Verb == "fade")
		//{
		//	float fadeTime  = command.TokenCount > 0 ? command.Tokens[0].Float : 1.f;
		//	float fadeDelay = command.TokenCount > 1 ? command.Tokens[1].Float : 0.f;
		//	float fadeTotalTime = fadeTime + fadeDelay;

		//	iw::Layer* fadeLayer = PushLayer<Menu_Fadeout_Layer>(fadeTime, fadeDelay);

		//	float time = iw::RawTotalTime();
		//	Task->coroutine(
		//		[this, time, fadeTotalTime, fadeLayer]()
		//	{
		//		bool done = iw::RawTotalTime() - time > fadeTotalTime;
		//		if (done) PopLayer(fadeLayer);
		//		return done;
		//	});
		//}

		else
		if (command.Verb == "set-state")
		{
			std::string name  = command.Str(0);
			float       delay = command.Num(1);

			float time = iw::TotalTime();
			Task->coroutine(
				[=]()
			{
				bool finished = iw::TotalTime() - time > delay;
				if (finished)
				{
					SetState(name);
				}

				return finished;
			});
		}

		//else
		//if (command.Verb == "push-state")
		//{
		//	std::string stateName = command.Tokens[0].String;

		//	     if (stateName == "settings")   PushState(MakeState_Settings());
		//	else if (stateName == "highscores") PushState(MakeState_Highscores());
		//	//else if (stateName == "settings") PushState(MakeState_Settings());

		//	else
		//	{
		//		LOG_ERROR << "[push-state] invalid state";
		//	}
		//}

		//else
		//if (command.Verb == "push-state")
		//{
		//	PopState();
		//}

		else
		if (command.Verb == "final-score")
		{
			//m_finalScore = command.Tokens[0].Int;
		}

		else
		if (command.Verb == "exit")
		{
			Stop();
		}

		return false;
	});

	return 0;
}

// new simple ecs testing

//template<std::size_t N>
//auto pack() {
//	return std::tuple_cat(std::tuple<int>{}, array_tuple<N - 1>());
//}
//
//template<>
//auto pack<0>() {
//	return std::tuple<>{};
//}
//
//template<typename _q, typename... _t>
//int tuple_offset()
//{
//	const std::tuple<_t...> instance; // remove this
//	return (char*)&std::get<_q, _t...>(instance)
//		 - (char*)&instance;
//}
//
//template<typename... _t, size_t... _index>
//std::tuple<_t&...> tie_from_offsets_i(
//	char* raw,
//	const std::array<int, sizeof...(_t)>& offsets,
//	std::index_sequence<_index...>)
//{
//	static_assert(sizeof...(_t) == sizeof...(_index));
//	return std::tuple<_t&...>(*(_t*)(raw + offsets[_index]) ...);
//}
//
//template<typename... _t>
//std::tuple<_t&...> tie_from_offsets(
//	char* raw,
//	const std::array<int, sizeof...(_t)>& offsets)
//{
//	return tie_from_offsets_i<_t...>(raw, offsets, std::make_index_sequence<sizeof...(_t)>{});
//}
//
//struct component
//{
//	int Hash;
//	int Size;
//	int Offset;
//};
//
//struct list
//{
//	iw::sparse_set<int>* m_indices;
//	std::vector<component> m_layout;
//
//	list(
//		iw::sparse_set<int>* indices,
//		std::vector<component> layout
//	) 
//		: m_indices (indices)
//		, m_layout  (layout)
//	{}
//
//	virtual char* raw_begin() const = 0;
//	virtual char* raw_end  () const = 0;
//	virtual int   item_size() const = 0;
//
//	template<typename... _q>
//	std::array<int, sizeof...(_q)> get_offsets()
//	{
//		std::array<int, sizeof...(_q)> offsets;
//
//		int i = 0;
//		for (int hash : { typeid(_q).hash_code()... })
//		{
//			for (const component& c : m_layout)
//			{
//				if (hash == c.Hash)
//				{
//					offsets[i++] = c.Offset;
//					break;
//				}
//			}
//		}
//
//		assert(i == sizeof...(_q));
//		return offsets;
//	}
//
//	template<typename... _q>
//	bool matches()
//	{
//		bool matched = false;
//		for (int hash : { typeid(_q).hash_code()... })
//		{
//			bool found = false;
//			if (const component & c : m_layout)
//			{
//				if (hash == c.Hash)
//				{
//					found = true;
//					break;
//				}
//			}
//
//			if (!found)
//			{
//				matched = false;
//				break;
//			}
//		}
//
//		return matched;
//	}
//
//	template<typename... _q>
//	struct itr
//	{
//		char* m_item;
//		int m_stride;
//		std::array<int, sizeof...(_q)> m_offsets;
//
//		itr (char* item, int stride, std::array<int, sizeof...(_q)>&& offsets)
//			: m_item    (item)
//			, m_stride  (stride)
//			, m_offsets (offsets)
//		{}
//
//		itr& operator++() { m_item += m_stride; return *this; }
//		std::tuple<_q&...> operator*() const { return tie_from_offsets<_q...>(m_item, m_offsets); }
//		bool operator==(const itr& other) const { return m_item == other.m_item; }
//		bool operator!=(const itr& other) const { return !operator==(other); }
//	};
//
//	template<typename... _q> itr<_q...> begin() { return itr<_q...>(raw_begin(), item_size(), get_offsets<_q...>()); }
//	template<typename... _q> itr<_q...> end()   { return itr<_q...>(raw_end(),   item_size(), get_offsets<_q...>()); }
//};
//
//template<int _s/*, typename... _t*/>
//struct list_with_types : list
//{
//	//iw::sparse_set<int, std::tuple<_t...>> m_items;
//
//	iw::sparse_set<int, std::array<char, _s>> m_data;
//
//	list_with_types()
//		: list(&m_items, { component { (int)typeid(_t).hash_code(), sizeof(_t), tuple_offset<_t, _t...>() }...})
//	{
//
//	}
//
//	char* raw_begin() const override { return (char*)m_items.m_items.begin()._Ptr;  };
//	char* raw_end  () const override { return (char*)m_items.m_items.end()  ._Ptr; };
//	int   item_size() const override { return (int)sizeof(std::tuple<_t...>); };
//
//
//
//	//template<typename _o>
//	//list_with_types<_o, _t...> prepend_type()
//	//{
//	//	return list_with_types<_o, _t...>();
//	//}
//};
//
//template<typename... _q>
//struct view
//{
//	std::vector<list*> m_matches;
//
//	
//};
//
//struct manager
//{
//	std::vector<list*> lists;
//
//	template<typename... _q>
//	view<_q...> query()
//	{
//		view<_q...> view;
//		for (list* list : lists)
//		{
//			if (list.matches<_q...>())
//			{
//				view.m_matches.push_back(list);
//			}
//		}
//
//		return view;
//	}
//
//	template<typename... _t>
//	void create_archetype()
//	{
//	}
//
//	template<typename _t>
//	void add_to_archetype()
//	{
//
//	}
//};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		816,
		1039,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}
