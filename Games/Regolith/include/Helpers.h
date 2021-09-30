#pragma once

#include "iw/engine/ComponentHelper.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include <string>
#include <sstream>
#include <utility>
#include <vector>

template<typename _t>
std::string tos(const _t& numb)
{
	std::stringstream buf;
	if (numb >= 0) buf << numb;
	else           buf << " "; // need space for UpdateMesh

	return buf.str();
}

inline std::tuple<float, float> randvel(
	iw::Rigidbody* rigidbody,
	float amount,
	bool setForMe = false) 
{
	float dx = rigidbody->Velocity.x;
	float dy = rigidbody->Velocity.y;

	float speed = sqrt(dx * dx + dy * dy);
	float angle = atan2(dy, dx);
	angle += iw::randfs() * amount;

	dx = cos(angle) * speed;
	dy = sin(angle) * speed;

	if (setForMe) {
		rigidbody->Velocity = glm::vec3(dx, dy, 0);
	}

	return std::tuple(dx, dy);
}

// lightning should prob but in seperate file

struct CellInfo
{
	int x;
	int y;
	iw::TileInfo tile;
};

inline std::vector<CellInfo> FindClosestCellPositionsMatchingTile(
	iw::SandLayer* sand,
	iw::Tile* tile,
	int x, int y)
{
	if (!tile)
	{
		return {};
	}

	std::vector<CellInfo> cells;
	int searchSize = 5;

	while (cells.size() == 0 && searchSize < tile->m_sprite.m_width) // should max with height
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
				if (info.tile == tile)
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

inline LightningHitInfo DrawLightning(
	iw::SandLayer* sand,
	float  x, float  y, 
	float tx, float ty,
	float arc,
	float life)
{
	float dx = tx - x;
	float dy = ty - y;
	float td = sqrt(dx*dx + dy*dy);
	float  d = td;

	float stepSize = 5;

	LightningHitInfo hit;

	iw::Cell c;
	c.Type = iw::CellType::LIGHTNING;

	c.Props = iw::CellProp::NONE/*MOVE_FORCE*/;
	c.life = life /*+ iw::randfs() * life / 2.f*/;
	//c.dx = 20 * (iw::randi() ? -1 : 1);
	//c.dy = 20 * (iw::randi() ? -1 : 1);

	c.Style = iw::CellStyle::SHIMMER;
	c.Color = iw::Color::From255(212, 194, 252);
	c.StyleColor = iw::Color(.1, .1, .1, 0);
	c.StyleOffset = iw::randfs() * .5;

	while (d > 10 && !hit.HasContact)
	{
		dx = tx - x;
		dy = ty - y;
      
		d = iw::max(1.f, sqrt(dx*dx + dy*dy));

		dx = dx / d * stepSize;
		dy = dy / d * stepSize;
      
		float left = d / td;
      
		if (left > 1)
		{
			left = .1;
		}

		float x1 = x + dx + left * iw::randfs() * arc;
		float y1 = y + dy + left * iw::randfs() * arc;
      
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
					|| info.tile)
				{
					hit.X = px;
					hit.Y = py;
					hit.TileInfo = info;
					hit.HasContact = true;

					return true;
				}

				sand->m_world->SetCell(px, py, c);
				
				return false;
			});
      
		x = x1;
		y = y1;
	}

	return hit;
}

inline LightningHitInfo DrawLightning(
	iw::SandLayer* sand,
	iw::ref<iw::Space>& space,
	iw::Entity originEntity,
	iw::Entity targetEntity)
{
	// pick points that are on surface

	iw::CollisionObject* originObj = iw::GetPhysicsComponent(space, originEntity.Handle);
	iw::CollisionObject* targetObj = iw::GetPhysicsComponent(space, targetEntity.Handle);

	glm::vec2 origin = originObj->Transform.Position;
	glm::vec2 target = targetObj->Transform.Position;

	glm::vec2 delta = target - origin;

	glm::vec2 a = originObj->Collider->as_dim<iw::d2>()->FindFurthestPoint(&originObj->Transform,  delta);
	glm::vec2 b = targetObj->Collider->as_dim<iw::d2>()->FindFurthestPoint(&targetObj->Transform, -delta);

	iw::Tile* originTile = originEntity.Find<iw::Tile>();
	iw::Tile* targetTile = targetEntity.Find<iw::Tile>();

	std::vector<CellInfo> origins = FindClosestCellPositionsMatchingTile(sand, originTile, a.x, a.y);
	std::vector<CellInfo> targets = FindClosestCellPositionsMatchingTile(sand, targetTile, b.x, b.y);

	if (    origins.size() == 0 
		|| targets.size() == 0)
	{
		return {};
	}

	CellInfo o = origins.at(iw::randi(origins.size() - 1));
	CellInfo t = targets.at(iw::randi(targets.size() - 1));

	return DrawLightning(sand, o.x, o.y, t.x, t.y, 10, .04);
}
