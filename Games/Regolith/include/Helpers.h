#pragma once

#include "iw/engine/ComponentHelper.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "plugins/iw/Sand/Tile.h"
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


		//e.on_add([](int) {});
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

enum class LightningType
{
	POINT,
	ENTITY
};

struct LightningConfig
{
	LightningType Type = LightningType::POINT; // only for outside logic
	entity A; // only used if Type = ENTITY
	entity B;
	entity Entity; // the entity who owns this lightning

	int X = 0;
	int Y = 0;
	int TargetX = 0;
	int TargetY = 0;
	float ArcSize = 0;
	float LifeTime = 0;
	bool StopOnHit = false;
};

inline
LightningHitInfo DrawLightning(
	iw::SandLayer* sand,
	LightningConfig config)
{
	float x = config.X;
	float y = config.Y;
	float dx = config.TargetX - x;
	float dy = config.TargetY - y;
	float td = sqrt(dx*dx + dy*dy);
	float  d = td;

	float stepSize = 5;

	LightningHitInfo hit;

	iw::Cell c = iw::Cell::GetDefault(iw::CellType::LIGHTNING);
	c.life = config.LifeTime;
	c.StyleOffset = iw::randfs() * .5;

	while (d > 2 && (!hit.HasContact || !config.StopOnHit))
	{
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
				c.life += 0.00005;
				//c.dx *= 0.99f * (iw::randi() ? -1 : 1);
				//c.dy *= 0.99f * (iw::randi() ? -1 : 1);

				if (!sand->m_world->InBounds(px, py))
				{
					return false; // bolt could come back into world
				}

				iw::Cell&     cell = sand->m_world->GetCell(px, py);
				iw::TileInfo& info = sand->m_world->GetChunk(px, py)->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);

				if (    cell.Type != iw::CellType::EMPTY
					&& cell.Type != iw::CellType::LIGHTNING
					|| (    info.tile
						&& info.tile != &config.A.get<iw::Tile>()))
				{
					hit.X = px;
					hit.Y = py;
					hit.TileInfo = info;
					hit.HasContact = true;

					return config.StopOnHit;
				}

				sand->m_world->SetCell(px, py, c);
				
				return false;
			});
      
		x = x1;
		y = y1;
	}

	return hit;
}

inline
LightningHitInfo DrawLightning(
	iw::SandLayer* sand,
	iw::ref<iw::Space>& space,
	LightningConfig config)
{
	// pick points that are on surface

	iw::CollisionObject* originObj = GetPhysicsComponent(config.A);
	iw::CollisionObject* targetObj = GetPhysicsComponent(config.B);

	if (!originObj || !targetObj)
	{
		return {};
	}

	glm::vec2 origin = originObj->Transform.Position;
	glm::vec2 target = targetObj->Transform.Position;

	glm::vec2 delta = target - origin;

	glm::vec2 a = originObj->Collider
	            ? originObj->Collider->as_dim<iw::d2>()->FindFurthestPoint(&originObj->Transform, delta)
	            : originObj->Transform.WorldPosition();

	glm::vec2 b = targetObj->Collider
	            ? targetObj->Collider->as_dim<iw::d2>()->FindFurthestPoint(&targetObj->Transform, -delta)
	            : targetObj->Transform.WorldPosition();

	std::vector<CellInfo> origins;
	std::vector<CellInfo> targets;

	if (config.A.has<iw::Tile>())
	{
		origins = FindClosestCellPositionsMatchingTile(sand, config.A.get<iw::Tile>(), a.x, a.y);
	}
	else
	{
		CellInfo& info = origins.emplace_back();
		info.x = (int)floor(a.x);
		info.y = (int)floor(a.y);
	}

	if (config.B.has<iw::Tile>())
	{
		targets = FindClosestCellPositionsMatchingTile(sand, config.B.get<iw::Tile>(), b.x, b.y);
	}
	else
	{
		CellInfo& info = targets.emplace_back();
		info.x = (int)floor(b.x);
		info.y = (int)floor(b.y);
	}

	if (    origins.size() == 0 
	     || targets.size() == 0)
	{
		return {};
	}

	CellInfo o = origins.at(iw::randi(origins.size() - 1));
	CellInfo t = targets.at(iw::randi(targets.size() - 1));

	config.X = o.x;
	config.Y = o.y;
	config.TargetX = t.x;
	config.TargetY = t.y;

	return DrawLightning(sand, config);
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

		LOG_INFO << "Copying from " << i << " to " << it;

		if (it > texture->ColorCount())
		{
			continue;
		}

		texture->Colors32()[it] = tile.m_sprite.Colors32()[i];
	}

	::entity split = MakeTile(*texture, others);

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

	splitBody.Velocity = o3; // could add vel of projectile
	splitBody.AngularVelocity.z = iw::randf();

	return split;
}
