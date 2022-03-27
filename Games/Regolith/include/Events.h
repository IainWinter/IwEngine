#pragma once

// new event system
// should focus on the order of function execution

// main goal:
// should allow for handlers to classes without an interface for every type of function
// and events arent an interface

#include <functional>
#include <unordered_map>
#include <vector>

using hash_t = uint64_t;

struct event_type
{
	hash_t m_hash = 0;
	int m_size = 0;
	const char* m_name = nullptr;
};

template<typename _e>
event_type make_event()
{
	static event_type e;
	if (e.m_hash == 0)
	{
		e.m_hash = typeid(_e).hash_code();
		e.m_size = sizeof(_e);
		e.m_name = typeid(_e).name();
	}

	return e;
}

struct event_pipe
{
	void* m_destination; // bound with type info in lambdas
	std::function<void(void*)> m_send;
	std::function<bool(void*)> m_only_if;

	void send(void* event)
	{
		if (!m_only_if || m_only_if(event))
		{
			m_send(event);
		}
	}
};

template<typename _e>
struct event_pipe_wrapper
{
	event_pipe& m_pipe;

	event_pipe_wrapper(event_pipe& pipe) : m_pipe(pipe) {}

	void only_if(std::function<bool(const _e&)> func)
	{
		m_pipe.m_only_if = [func](void* event)
		{
			return func(*(_e*)event);
		};
	}
};

template<typename _e, typename _h>
event_pipe make_pipe(_h* handler)
{
	//static_assert(decltype(_h::on(_t&)));

	event_pipe pipe;
	pipe.m_destination = handler;
	pipe.m_send = [=](void* event)
	{
		handler->on(*(_e*)event);
	};

	return pipe;
}

struct event_sink
{
	event_type m_type;
	std::vector<event_pipe> m_pipes;

	template<typename _e, typename _h>
	event_pipe& add_pipe(_h* handler)
	{
		return m_pipes.emplace_back(make_pipe<_e, _h>(handler));
	}

	void remove_pipe(void* handler)
	{
		for (int i = 0; i < m_pipes.size(); i++)
		{
			if (m_pipes.at(i).m_destination == handler)
			{
				m_pipes.erase(m_pipes.begin() + i);
				--i; // dont break could have multiple pipes to same instance
			}
		}
	}

	void send(void* event)
	{
		for (event_pipe& pipe : m_pipes)
		{
			pipe.send(event);
		}
	}
};

struct event_manager
{
	std::unordered_map<hash_t, event_sink> m_sinks;

	// needs to be template

	template<typename _e, typename _h>
	event_pipe_wrapper<_e> handle(_h* handler_ptr)
	{
		event_sink& sink = m_sinks[make_event<_e>().m_hash];
		event_pipe& pipe = sink.add_pipe<_e, _h>(handler_ptr);
		return event_pipe_wrapper<_e>(pipe);
	}

	void unhandle(void* handler)
	{
		for (auto itr = m_sinks.begin(); itr != m_sinks.end();)
		{
			itr->second.remove_pipe(handler);

			if (itr->second.m_pipes.size() == 0)
			{
				itr = m_sinks.erase(itr);
			}
			else
			{
				++itr;
			}
		}
	}

	void unhandle(event_type type, void* handler)
	{
		event_sink& sink = m_sinks.at(type.m_hash);
		sink.remove_pipe(handler);

		if (sink.m_pipes.size() == 0)
		{
			m_sinks.erase(type.m_hash);
		}
	}

	void send(event_type type, void* event)
	{
		auto itr = m_sinks.find(type.m_hash);
		if (itr != m_sinks.end())
		{
			itr->second.send(event);
		}
	}

	// template headers

	template<typename _t, typename _h>
	void unhandle(_h* handler_ptr)
	{
		unhandle(make_event<_t>(), handler_ptr);
	}

	template<typename _t>
	void send(_t&& event)
	{
		send(make_event<_t>(), (void*)&event);
	}
};

struct event_queue
{
	struct queued_event_base
	{
		virtual ~queued_event_base() = default;
		virtual void send(event_manager* manager) = 0;
	};

	template<typename _e>
	struct queued_event : queued_event_base
	{
		const char* m_where;
		event_type m_type;
		_e m_event;

		void send(event_manager* manager)
		{
			manager->send(m_type, &m_event);
		}
	};

	event_manager* m_manager;
	std::vector<queued_event_base*> m_queue;
	const char* m_where_current;

	event_queue(
		event_manager* manager
	)
		: m_manager       (manager)
		, m_where_current (nullptr)
	{}

	template<typename _e>
	void queue(_e&& event)
	{
		queued_event<_e>* qe = new queued_event<_e>();
		qe->m_type = make_event<_e>();
		qe->m_event = std::forward<_e>(event);
		qe->m_where = m_where_current;
		m_queue.push_back(qe);
	}

	template<typename _e>
	void send(_e&& event)
	{
		m_manager->send<_e>(std::forward<_e>(event));
	}

	void execute()
	{
		std::vector<queued_event_base*> copy = m_queue;
		for (queued_event_base* event : copy)
		{
			event->send(m_manager);
			delete event;
		}
		m_queue.clear();
	}
};

/*
	If you want to use singletons, note dll bounds
*/
#define USE_SINGLETONS

#ifdef USE_SINGLETONS
	inline
	event_manager& events()
	{
		static event_manager manager;
		return manager;
	}

	inline
	event_queue& events_defer(const char* where_from)
	{
		static event_queue queue(&events());
		queue.m_where_current = where_from;
		return queue;
	}

	/*
		If you want to log the location of the dispatcher
	*/
#	define LOG_DEFER

#	ifdef LOG_DEFER
#		ifndef iw_as_string
#			define iw_line_as_string(x) iw_as_string(x)
#			define iw_as_string(x) #x
#		endif
#		define events_defer() events_defer(__FUNCTION__ " " iw_line_as_string(__LINE__))
#	endif
#endif

























#include "iw/engine/Events/ActionEvents.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Color.h"
#include "plugins/iw/Sand/Defs.h"
#include "Components/Player.h"
#include "Components/Item.h"
#include "Components/Projectile.h"
#include "Components/EnemyShip.h"
#include "iw/util/reflection/ReflectDef.h"
#include "State.h"
#include "Upgrade.h"
#include "ECS.h"

enum Actions {
	SPAWN_ENEMY,
	SPAWN_PROJECTILE,
	SPAWN_ITEM,
	SPAWN_ASTEROID,
	SPAWN_EXPLOSION,
	CHANGE_LASER_FLUID,
	CHANGE_PLAYER_WEAPON,
	CHANGE_PLAYER_SCORE,
	HEAL_PLAYER,
	PROJ_HIT_TILE,
	REMOVE_CELL_FROM_TILE,
	CREATED_CORE_TILE,
	CORE_EXPLODED,
	STATE_CHANGE,
	OPEN_MENU,

	SET_CELL,

	CREATED_PLAYER,
	DESTROYED_PLAYER,

	APPLY_UPGRADE_SET,

	PLAY_SOUND,
};

struct REFLECT SpawnExplosion_Config
{
	float SpawnLocationX  =   0.f;
	float SpawnLocationY  =   0.f;
	float ExplosionRadius =  10.f;
	float ExplosionPower  = 200.f;
};

struct REFLECT SpawnExplosion_Event : iw::SingleEvent
{
	SpawnExplosion_Config Config;

	SpawnExplosion_Event(
		SpawnExplosion_Config& config
	)
		: iw::SingleEvent(SPAWN_EXPLOSION)
		, Config(config)
	{}
};

struct REFLECT SpawnEnemy_Config
{
	entity TargetEntity;
	float SpawnLocationX  = 0.f;
	float SpawnLocationY  = 0.f;
	float TargetLocationX = 0.f;
	float TargetLocationY = 0.f;

	EnemyType EnemyType = EnemyType::FIGHTER;
};

struct REFLECT SpawnEnemy_Event : iw::SingleEvent
{
	SpawnEnemy_Config Config;

	SpawnEnemy_Event(
		SpawnEnemy_Config config
	)
		: iw::SingleEvent (SPAWN_ENEMY)
		, Config          (config)
	{}
};

struct REFLECT SpawnAsteroid_Config
{
	float SpawnLocationX = 0.f;
	float SpawnLocationY = 0.f;
	float VelocityX      = 0.f;
	float VelocityY      = 0.f;
	float AngularVel     = 0.f;
	int Size = 1;
	float Mass = 1000.f;
	//bool DrawAsBox = false;
	bool DontRemoveCells = false;

	std::string TextureOverride = "";
};

struct REFLECT SpawnAsteroid_Event : iw::SingleEvent
{
	SpawnAsteroid_Config Config;

	SpawnAsteroid_Event(
		const SpawnAsteroid_Config& config
	)
		: iw::SingleEvent(SPAWN_ASTEROID)
		, Config(config)
	{}
};

struct REFLECT SpawnProjectile_Event : iw::SingleEvent
{
	ShotInfo Shot;
	int Depth;

	SpawnProjectile_Event(
		const ShotInfo& shot,
		int depth = 0
	)
		: iw::SingleEvent(SPAWN_PROJECTILE)
		, Shot(shot)
		, Depth(depth)
	{}
};

struct REFLECT SpawnItem_Config
{
	float X             =  0.f; // should be named SpawnLocationX
	float Y             =  0.f;
	float ActivateDelay =  0.f;
	float AngularSpeed  =  0.f;
	float Speed         = 75.f;

	int Amount          = 1;

	ItemType Item;
	std::function<void()> OnPickup;
};

struct REFLECT SpawnItem_Event : iw::SingleEvent
{
	SpawnItem_Config Config;

	SpawnItem_Event(
		const SpawnItem_Config& config
	)
		: iw::SingleEvent(SPAWN_ITEM)
		, Config(config)
	{}
};

struct REFLECT ChangeLaserFluid_Event : iw::SingleEvent
{
	int Amount;

	ChangeLaserFluid_Event(
		int amount
	)
		: iw::SingleEvent(CHANGE_LASER_FLUID)
		, Amount(amount)
	{}
};

struct REFLECT ChangePlayerWeapon_Event : iw::SingleEvent
{
	WeaponType Type;
	Weapon* CurrentWeapon;

	ChangePlayerWeapon_Event(
		WeaponType weapon
	)
		: iw::SingleEvent(CHANGE_PLAYER_WEAPON)
		, Type(weapon)
		, CurrentWeapon(nullptr)
	{}
};

struct REFLECT ChangePlayerScore_Event : iw::SingleEvent
{
	int Score;

	ChangePlayerScore_Event(
		int score
	)
		: iw::SingleEvent(CHANGE_PLAYER_SCORE)
		, Score(score)
	{}
};

struct REFLECT HealPlayer_Event : iw::SingleEvent
{
	bool IsCore;

	HealPlayer_Event(
		int isCore = false
	)
		: iw::SingleEvent(HEAL_PLAYER)
		, IsCore(isCore)
	{}
};

struct REFLECT ProjHitTile_Config
{
	int X = 0;
	int Y = 0;
	iw::TileInfo Info;
	entity Hit;
	entity Projectile;
};

struct REFLECT ProjHitTile_Event : iw::SingleEvent
{
	ProjHitTile_Config Config;
	
	ProjHitTile_Event(
		const ProjHitTile_Config& config
	)
		: iw::SingleEvent(PROJ_HIT_TILE)
		, Config(config)
	{}
};

struct REFLECT RemoveCellFromTile_Event : iw::SingleEvent
{
	size_t Index;
	entity Entity;

	RemoveCellFromTile_Event(
		size_t index, entity entity
	)
		: iw::SingleEvent (REMOVE_CELL_FROM_TILE)
		, Index           (index)
		, Entity          (entity)
	{}
};

struct REFLECT CreatedPlayer_Event : iw::SingleEvent
{
	entity PlayerEntity;

	CreatedPlayer_Event(
		entity playerEntity
	)
		: iw::SingleEvent (CREATED_PLAYER)
		, PlayerEntity    (playerEntity)
	{}
};

struct REFLECT CreatedCoreTile_Event : iw::SingleEvent
{
	entity TileEntity;

	CreatedCoreTile_Event(
		entity tileEntity
	)
		: iw::SingleEvent (CREATED_CORE_TILE)
		, TileEntity      (tileEntity)
	{}
};


struct REFLECT CoreExploded_Event : iw::SingleEvent
{
	entity Entity;

	CoreExploded_Event(
		entity entity
	)
		: iw::SingleEvent (CORE_EXPLODED)
		, Entity          (entity)
	{}
};

struct REFLECT StateChange_Event : iw::SingleEvent
{
	StateName State;

	StateChange_Event(
		StateName state
	)
		: iw::SingleEvent (STATE_CHANGE)
		, State           (state)
	{}
};

enum MenuName {
	ECS_MENU
};

struct REFLECT OpenMenu_Event : iw::SingleEvent
{
	MenuName Menu;

	OpenMenu_Event(
		MenuName menu
	)
		: iw::SingleEvent (OPEN_MENU)
		, Menu            (menu)
	{}
};

struct /*REFLECT*/ ApplyUpgradeSet_Event : iw::SingleEvent
{
	UpgradeSet Set;

	ApplyUpgradeSet_Event(
		const UpgradeSet& set
	)
		: iw::SingleEvent (APPLY_UPGRADE_SET)
		, Set             (set)
	{}
};

struct /*REFLECT*/ SetCell_Event : iw::SingleEvent
{
	iw::Cell cell;

	SetCell_Event(
		const iw::Cell& c
	)
		: iw::SingleEvent (SET_CELL)
		, cell(c)
	{}
};

struct /*REFLECT*/ PlaySound_Event : iw::SingleEvent
{
	std::string Name;
	std::unordered_map<std::string, float> Parameters;

	PlaySound_Event(
		const std::string& name
	)
		: iw::SingleEvent (PLAY_SOUND)
		, Name            (name)
	{}

	PlaySound_Event(
		const std::string& name,
		float impulse
	)
		: iw::SingleEvent (PLAY_SOUND)
		, Name            (name)
	{
		Parameters["Impulse"] = impulse;
	}
};
