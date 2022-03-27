#pragma once

#include "iw/engine/ComponentHelper.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "plugins/iw/Sand/Tile.h"
#include "iw/util/thread/thread_pool.h"
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_set>
#include "ECS.h"

template<typename _t>
std::string tos(const _t& numb)
{
	std::stringstream buf;
	if (numb >= 0) buf << numb;
	else           buf << " "; // need space for UpdateMesh

	return buf.str();
}

inline std::tuple<float, float> randvel(
	iw::Rigidbody& rigidbody,
	float amount,
	bool setForMe = false) 
{
	float dx = rigidbody.Velocity.x;
	float dy = rigidbody.Velocity.y;

	float speed = sqrt(dx * dx + dy * dy);
	float angle = atan2(dy, dx);
	angle += iw::randfs() * amount;

	dx = cos(angle) * speed;
	dy = sin(angle) * speed;

	if (setForMe) {
		rigidbody.Velocity = glm::vec3(dx, dy, 0);
	}

	return std::tuple(dx, dy);
}

// more from engine just for new entities

#include "iw/entity/Space.h"
#include "iw/physics/Collision/Manifold.h"


// Tries to find entities with CollisionObject / Rigidbody components.
// Returns true if there were no entities.
// if _t2 is specified, checks if that component exists on entity 2
template<
	typename _t1,
	typename _t2 = void>
bool GetEntitiesFromManifold(
	const iw::Manifold& manifold,
	entity& e1,
	entity& e2)
{
	auto a = GetPhysicsEntity(manifold.ObjA);
	auto b = GetPhysicsEntity(manifold.ObjB);

	if (!a.first || !b.first) return true; // no physics components

	bool a1 = a.second.has<_t1>();
	bool b1 = b.second.has<_t1>();

	if (!a1 && !b1) return true; // no t1

	if (b1) { // b has t1
		entity t = a;
		a = b;
		b = t;
	}

	if constexpr (std::is_same_v<_t2, void> == false)
	{
		if (!b.second.has<_t2>()) { // no t2
			return true;
		}
	}

	e1 = a;
	e2 = b;

	return false;
}

inline
iw::CollisionObject* GetPhysicsComponent(
	entity e)
{
	if (e.has<iw::Rigidbody>())
	{
		return &e.get<iw::Rigidbody>();
	}

	if (e.has<iw::CollisionObject>())
	{
		return &e.get<iw::CollisionObject>();
	}

	return nullptr;
}

inline
std::pair<bool, entity> GetPhysicsEntity(
	iw::CollisionObject* object)
{
	auto a = entities().find(object);
	if (!a.first) a = entities().find((iw::Rigidbody*)object);
	return a;
}

inline
void AddEntityToPhysics(
	entity e,
	iw::ref<iw::DynamicsSpace>& physics)
{
	if (e.has<iw::Rigidbody>())
	{
		iw::Rigidbody* ptr = &e.get<iw::Rigidbody>();
		physics->AddRigidbody(&e.get<iw::Rigidbody>());

		e.on_add([=](entity entity) {
			physics->RemoveCollisionObject(ptr);
			physics->AddRigidbody(&entity.get<iw::Rigidbody>());
		});
	}

	else
	if (e.has<iw::CollisionObject>())
	{
		iw::CollisionObject* ptr = &e.get<iw::CollisionObject>();
		physics->AddCollisionObject(ptr);

		e.on_add([=](entity entity) {
			physics->RemoveCollisionObject(ptr);
			physics->AddCollisionObject(&entity.get<iw::CollisionObject>());
		});
	}

	e.on_destroy([=](entity entity) {
		physics->RemoveCollisionObject(GetPhysicsComponent(entity));
	});
}

// lightning should prob but in seperate file

struct CellInfo
{
	int x;
	int y;
	iw::TileInfo tile;
};

inline
std::vector<CellInfo> FindClosestCellPositionsMatchingTile(
	iw::SandLayer* sand,
	iw::Tile& tile,
	int x, int y)
{
	std::vector<CellInfo> cells;
	int searchSize = 5;

	while (cells.size() == 0 && searchSize < tile.m_sprite.m_width) // should max with height
	{
		for (int sy = -searchSize; sy < searchSize; sy++) // could be more efficient
		for (int sx = -searchSize; sx < searchSize; sx++)
		{
			int px = sx + x;
			int py = sy + y;

			iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);

			if (chunk)
			{
				iw::TileInfo& info = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);

				if (info.tile == &tile)
				{
					CellInfo ci;
					ci.x = px;
					ci.y = py;
					ci.tile = info;
					cells.push_back(ci);
				}
			}
		}

		searchSize *= 2;
	}

	return cells;
}

struct LightningHitInfo
{
	int X = 0;
	int Y = 0;
	iw::TileInfo TileInfo;
	bool HasContact = false;
};

struct LightningConfig
{
	entity A;
	entity B;
	entity Owner;

	int X = 0;
	int Y = 0;
	int TargetX = 0;
	int TargetY = 0;
	float ArcSize = 0;
	float LifeTime = 0.005f;
	bool StopOnHit = false;

	float DelayPerCell = 0.f;

	iw::ref<iw::Space> Space; // for finding point on tile
	iw::ref<iw::thread_pool> Task; // for delayed draw
};

inline
CellInfo GetRandomNearCell(iw::SandLayer* sand, entity entity, glm::vec2 delta)
{
	iw::CollisionObject* obj = entity.is_alive() ? GetPhysicsComponent(entity) : nullptr;

	glm::vec2 a = obj->Collider
	            ? obj->Collider->as_dim<iw::d2>()->FindFurthestPoint(&obj->Transform, delta)
	            : obj->Transform.WorldPosition();

	std::vector<CellInfo> cells;

	if (entity.has<iw::Tile>())
	{
		cells = FindClosestCellPositionsMatchingTile(sand, entity.get<iw::Tile>(), a.x, a.y);
	}

	if (cells.size() > 0)
	{
		return cells.at(iw::randi(cells.size() - 1));
	}

	CellInfo info;
	info.x = (int)floor(a.x);
	info.y = (int)floor(a.y);

	return info;
}

inline
std::pair<LightningHitInfo, std::vector<std::pair<int, int>>> GetDrawLightningList(
	iw::SandLayer* sand,
	LightningConfig config)
{
	if (config.Space)
	{
		iw::CollisionObject* originObj = config.A.is_alive() ? GetPhysicsComponent(config.A) : nullptr;
		iw::CollisionObject* targetObj = config.B.is_alive() ? GetPhysicsComponent(config.B) : nullptr;

		glm::vec2 origin(config.X, config.Y);
		glm::vec2 target(config.TargetX, config.TargetY);

		if (originObj) origin = originObj->Transform.Position;
		if (targetObj) target = targetObj->Transform.Position;

		glm::vec2 delta = target - origin;

		if (originObj)
		{
			CellInfo cell = GetRandomNearCell(sand, config.A, delta);
			config.X = cell.x;
			config.Y = cell.y;
		}

		if (targetObj)
		{
			CellInfo cell = GetRandomNearCell(sand, config.A, delta);
			config.TargetX = cell.x;
			config.TargetY = cell.y;
		}
	}

	float x = config.X;
	float y = config.Y;
	float dx = config.TargetX - x;
	float dy = config.TargetY - y;
	float td = sqrt(dx*dx + dy*dy);
	float  d = td;

	float stepSize = 5;

	LightningHitInfo hit;

	std::vector<std::pair<int, int>> drawList;

	int max_iter = 1000;
	while (max_iter > 0 && d > 2 && (!hit.HasContact || !config.StopOnHit))
	{
		max_iter -= 1;

		dx = config.TargetX - x;
		dy = config.TargetY - y;
      
		d = iw::max(1.f, sqrt(dx*dx + dy*dy));

		dx = dx / d * stepSize;
		dy = dy / d * stepSize;
      
		float left = d / td;
      
		if (left > 1)
		{
			left = .1;
		}

		float x1 = x + dx + left * iw::randfs() * config.ArcSize;
		float y1 = y + dy + left * iw::randfs() * config.ArcSize;

		sand->ForEachInLine(x, y, x1, y1,
			[&](
				float px, float py)
			{
				if (!sand->m_world->InBounds(px, py))
				{
					return false; // bolt could come back into world
				}

				iw::Cell&     cell = sand->m_world->GetCell(px, py);
				iw::TileInfo& info = sand->m_world->GetChunk(px, py)->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);

				if (    cell.Type != iw::CellType::EMPTY
					&& cell.Type != iw::CellType::LIGHTNING
					|| info.tile)
				{
					hit.X = px;
					hit.Y = py;
					hit.TileInfo = info;
					hit.HasContact = true;

					return config.StopOnHit;
				}

				drawList.emplace_back(px, py);

				return false;
			});
      
		x = x1;
		y = y1;
	}

	return { hit, drawList };
}

inline
LightningHitInfo DrawLightning(
	iw::SandLayer* sand,
	LightningConfig config)
{
	auto [hit, drawList] = GetDrawLightningList(sand, config);

	iw::Cell c = iw::Cell::GetDefault(iw::CellType::LIGHTNING);
	c.life = config.LifeTime;
	c.StyleOffset = iw::randfs() * .5;

	if (config.Task && config.DelayPerCell > 0.f)
	{
		std::vector<std::pair<int, int>>* vec = new std::vector<std::pair<int, int>>(std::move(drawList));
		int* index = new int(0);
		float* timer = new float(0.f);
		float delay = config.DelayPerCell;

		c.life = delay * vec->size();

		config.Task->coroutine(
			[vec, index, timer, delay, sand, c]()
			{
				if (*index < vec->size())
				{
					*timer += iw::DeltaTime();

					while (*timer > delay && *index < vec->size())
					{
						*timer -= delay;

						const auto& [x, y] = vec->at(*index);
						sand->m_world->SetCell(x, y, c);
						*index += 1;
					}

					return false;
				}

				delete vec;
				delete index;
				delete timer;
				return true;
			}
		);

		hit.HasContact = false; // delayed lightning needs to use a lambda in the coroutine
	}

	else
	{
		for (const auto& [x, y] : drawList)
		{
			sand->m_world->SetCell(x, y, c);
		}
	}

	return hit;
}


enum cell_state : char
{
	EMPTY,
	FILLED = 100,
	VISITED = 10
};

inline
std::vector<cell_state> GetTileStates(
	iw::Tile& tile)
{
	size_t width  = tile.m_sprite.m_width;
	size_t height = tile.m_sprite.m_height;

	std::vector<cell_state> states;
	states.resize(width * height);

	for (const int& index : tile.m_currentCells)
	{
		states.at(index) = tile.State(index) == iw::Tile::FILLED 
			? cell_state::FILLED 
			: cell_state::EMPTY;
	}

	return states;
}

inline
void flood_fill(
	int seed,
	int size_x, int size_y,
	std::vector<cell_state>& cells,
	std::function<void(int)> onSet)
{
	std::vector<int> queue;
	queue.push_back(seed);

	while (queue.size() > 0)
	{
		int index = queue.back(); queue.pop_back();

		if (    index < 0 
			|| index >= size_x * size_y) 
		{
			continue;
		}

		if (cells[index] == FILLED)
		{
			cells[index] = VISITED;

			if (onSet)
			{
				onSet(index);
			}

			bool natLeft  = index % size_x != 0;
			bool natRight = index % size_x != size_x - 1;

			if (natRight) queue.push_back(index + 1);
			if (natLeft)  queue.push_back(index - 1);
					    queue.push_back(index + size_x);
					    queue.push_back(index - size_x);
		
			// diags

			if (natRight) queue.push_back(index + 1 + size_x);
			if (natRight) queue.push_back(index + 1 - size_x);
			if (natLeft)  queue.push_back(index - 1 + size_x);
			if (natLeft)  queue.push_back(index - 1 - size_x);
		}
	}
}



// move this for ECS test, should move back to sand, but not in any class

#include "iw/physics/Collision/MeshCollider.h"
#include "iw/graphics/Texture.h"

// this assumes that if others is provided, then the last component is a valid physics collider type
inline
entity MakeTile(
	int index,
	const iw::Texture& sprite,
	std::vector<component> others = {})
{
	if (others.size() > 0)
	{
		// insert at from to keep collider at the back
		others.insert(others.begin(), make_component<iw::Rigidbody>());
	}

	archetype arch = make_archetype<iw::Transform, iw::Tile>();
	arch = archetype_add(arch, others);

	::entity entity = entities().create(arch).set<iw::Tile>(sprite);
	
	iw::Transform& transform = entity.get<iw::Transform>();

	if (others.size() > 0)
	{
		iw::Rigidbody& body = entity.get<iw::Rigidbody>();
		body.SetTransform(&transform);

		if (others.back().m_hash == make_component<iw::Circle>().m_hash)
		{
			iw::Circle& circle = entity.get<iw::Circle>();
			circle.Radius = glm::compMax(sprite.Dimensions()) / 2.f;
		}

		entities().get(entity.m_handle, others.back(), (void**)&body.Collider);
	}

	entity.get<iw::Tile>().m_sandLayerIndex = index;

	return entity;
}

inline
entity SplitTile(
	entity& entity,
	std::vector<int> indices,
	const std::vector<component>& others)
{
	int minX =  INT_MAX;
	int minY =  INT_MAX;
	int maxX = -INT_MAX;
	int maxY = -INT_MAX;

	iw::Tile& tile = entity.get<iw::Tile>();

	for (int& i : indices)
	{
		auto [x, y] = iw::xy(i, (int)tile.m_sprite.m_width);

		if (x < minX) minX = x;
		if (y < minY) minY = y;
		if (x > maxX) maxX = x;
		if (y > maxY) maxY = y;
	}

	iw::ref<iw::Texture> texture = REF<iw::Texture>(maxX - minX + 1, maxY - minY + 1);
	texture->CreateColors();

	for (int& i : indices)
	{
		auto [x, y] = iw::xy(i, (int)tile.m_sprite.m_width);
		
		int it = (x - minX) + (y - minY) * texture->m_width;

		//LOG_INFO << "Copying from " << i << " to " << it;

		if (it > texture->ColorCount())
		{
			continue;
		}

		texture->Colors32()[it] = tile.m_sprite.Colors32()[i];
	}

	::entity split = MakeTile(tile.m_sandLayerIndex, *texture, others);

	// give split the pos and rot of the orignal peice

	iw::Transform& transform = entity.get<iw::Transform>();
	iw::Rigidbody& rigidbody = entity.get<iw::Rigidbody>();

	glm::vec2 midOld = entity.get<iw::Tile>().m_sprite.Dimensions() / 2.f;
	glm::vec2 midNew = split .get<iw::Tile>().m_sprite.Dimensions() / 2.f;
	glm::vec2 offset = iw::TransformPoint<iw::d2>(
			glm::vec2(
				maxX - floor(midOld.x) - floor(midNew.x),
				maxY - floor(midOld.y) - floor(midNew.y)
			),
			&transform.ScaleAndRotation()
		);

	iw::Transform& splitTran = split.get<iw::Transform>();
	iw::Rigidbody& splitBody = split.get<iw::Rigidbody>();

	glm::vec3 o3 = glm::vec3(offset, 0.f);

	splitTran = transform;
	splitTran.Position += o3;
	splitBody.SetTransform(&splitTran);

	// adjust masses to ratio of pixels

	float mass = rigidbody.Mass();
	float massRatio = indices.size() / (float)tile.m_currentCells.size();

	float splitMass = mass * massRatio;

	rigidbody.SetMass(mass - splitMass);
	splitBody.SetMass(splitMass);

	// not required for position

	glm::vec3 force  = o3 * 1000.f;
	glm::vec3 torque = glm::vec3(0.f, 0.f, iw::randf() * 10.f);

	splitBody.ApplyForce(force); // could add vel of projectile, also find what the constant here should be
	splitBody.ApplyTorque(torque);

	// add opposite force to current entity

	rigidbody.ApplyForce(-force);
	//rigidbody.ApplyTorque(-torque);

	return split;
}
