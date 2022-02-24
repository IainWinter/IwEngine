#include "Systems/TileSplit_System.h"

//void TileSplitSystem::OnPush()
//{
//	splitThreadRunning = true;
//	splitThread = std::thread([&]()
//	{
//		while (splitThreadRunning)
//		{
//			iw::Entity entity = Space->GetEntity(splits.pop());
//			if (entity.Alive())
//			{
//				SplitTile(entity);
//			}
//		}
//	});
//}
//
//void TileSplitSystem::OnPop()
//{
//	splitThreadRunning = false;
//	splitThread.join();
//}

void TileSplitSystem::Update()
{
	Task->defer([=]()
	{
		for (entity_handle handle : splits)
		{
			SplitTile(entities().wrap(handle));
		}
		splits.clear();
	});

	for (auto [entity, tile, asteroid] : entities().query<iw::Tile, Asteroid>().with_entity())
	{
		if (tile.m_currentCells.size() < 50) 
		{
			if (asteroid.Lifetime < 0.f)
			{
				ExplodeTile(entity, tile.m_currentCells);
				entities_defer().destroy(entity);
			}
		}
	}
}

bool TileSplitSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case REMOVE_CELL_FROM_TILE:
		{
			RemoveCellFromTile_Event& event = e.as<RemoveCellFromTile_Event>();
			entity_handle handle = event.Entity.m_handle;

			if (std::find(splits.begin(), splits.end(), handle) == splits.end())
			{
				splits.push(handle);
			}

			break;
		}
	}

	return false;
}

void TileSplitSystem::SplitTile(
	entity entity)
{
	if (!entity.is_alive()) return;

	iw::Tile& tile = entity.get<iw::Tile>();

	// Remove cells that are no longer connected to core

	size_t width  = tile.m_sprite.m_width;
	size_t height = tile.m_sprite.m_height;
	std::vector<cell_state> states = GetTileStates(tile);

	std::unordered_set<int> indices;
	if (entity.has<CorePixels>())
	{
		CorePixels& core = entity.get<CorePixels>();
			
		for (const int& seed : core.ActiveIndices)
		{
			flood_fill(seed, width, height, states, 
				[&indices](int index) {
					indices.insert(index);
				}
			);
		}

		// remove cells active in tile not in indices

		std::vector<int> toSplit;
		for (int i = 0; i < tile.m_currentCells.size(); i++)
		{
			int index = tile.m_currentCells.at(i);

			if (   indices           .find(index) == indices           .end()
				&& core.ActiveIndices.find(index) == core.ActiveIndices.end())
			{
				toSplit.push_back(index);
			}
		}
			
		if (toSplit.size() > 0) // not really needed but makes flow clear
		{
			SplitTileOff(entity, toSplit);

			for (int i : toSplit)
			{
				tile.RemovePixel(i);
			}
		}
	}

	else if (tile.m_currentCells.size() < 10)
	{
		ExplodeTile(entity, tile.m_currentCells);
		entities_defer().destroy(entity);
	}

	else
	{
		std::vector<std::vector<int>> islands;

		for (const int& seed : tile.m_currentCells)
		{
			std::vector<int> island;

			flood_fill(seed, width, height, states, 
				[&island](int index) {
					island.push_back(index);
				}
			);

			if (island.size() > 0)
			{
				islands.emplace_back(std::move(island));
			}
		}

		// only split if more than 1 island
		if (islands.size() > 1)
		{
			// find max island size
			size_t maxSize = 0, maxIndex = 0;
			for (size_t i = 0l; i < islands.size(); i++)
			{
				auto island = islands.at(i);

				if (island.size() > maxSize)
				{
					maxSize  = island.size();
					maxIndex = i;
				}
			}

			// true if should just split,
			// false if should delete old entity and spawn new ones

			bool justCut = maxSize * 2 > tile.m_currentCells.size();

			// remove spawn of main island
			if (justCut)
			{
				LOG_INFO << " ----- Cut tile ---";
				islands.erase(islands.begin() + maxIndex);
			}

			// remove main entity because we are spawning
			// the main island as another entity
			else
			{
				LOG_INFO << " ----- Split tile ---";
				entities_defer().destroy(entity);
			}

			for (const std::vector<int>& island : islands)
			{
				SplitTileOff(entity, island);

				// remove pixels from tile if just cutting off islands
				if (justCut)
				{
					for (int i : island)
					{
						tile.RemovePixel(i);
					}
				}
			}
		}

		// split or crack tile
		// this could use another component to keep track of a super simple
		// model of stress and fracture into multiple peices or perfectly
		// cut into two

		// could store fracture lines by taking a projectiles velocity/impact force
		// and doing a random walk throug a cone that gets narrower the faster the
		// projectile goes, so for the sniper that would cut in half while an explosion
		// would only fracture small peices off
	}
}

void TileSplitSystem::SplitTileOff(
	entity entity, 
	std::vector<int> toSplit)
{
	iw::Tile& tile = entity.get<iw::Tile>();
	glm::vec2 midOld = tile.m_sprite.Dimensions() / 2.f;
	if (toSplit.size() > 10)
	{
		::entity split = ::SplitTile(entity, toSplit, component_list<Asteroid, Throwable, iw::MeshCollider2>());
		AddEntityToPhysics(split, Physics);

		split.get<Asteroid>().Lifetime = toSplit.size() / 5.f;
	}

	else
	{
		ExplodeTile(entity, toSplit);
	}
}

void TileSplitSystem::ExplodeTile(
	entity entity,
	std::vector<int> toSplit)
{
	iw::Rigidbody& body = entity.get<iw::Rigidbody>();
	iw::Tile& tile = entity.get<iw::Tile>();
	glm::vec2 mid = entity.get<iw::Tile>().m_sprite.Dimensions() / 2.f;
	
	// make sparks the color of the cells
	for (int index : toSplit)
	{
		auto [x, y] = iw::xy<int>(index, tile.m_sprite.m_width);

		glm::vec2 offset = glm::vec2(
			x - floor(mid.x),
			y - floor(mid.y));

		glm::vec2 pos = iw::TransformPoint<iw::d2>(
			offset,
			&entity.get<iw::Transform>());

		iw::Cell spark;
		spark.Type  = iw::CellType::ROCK;
		spark.Props = iw::CellProp::MOVE_FORCE;
				
		spark.Color = iw::Color::From32(tile.m_sprite.Colors32()[index]);
				
		spark.dx = offset.x + iw::randfs() * 20 + body.Velocity.x * 2.f;
		spark.dy = offset.y + iw::randfs() * 20 + body.Velocity.y * 2.f;
		spark.life = .2 + iw::randf() * .2;

		float px = pos.x;
		float py = pos.y;
				
		sand->m_world->SetCell(px, py, spark);
	}
}

// debug
//for (unsigned i = 0; i < split.Find<iw::Tile>()->m_sprite.ColorCount32(); i++)
//{
//	unsigned& c = split.Find<iw::Tile>()->m_sprite.Colors32()[i];
//	if (c == 0)
//	{
//		c = iw::Color(1, 1, 1, .5).to32();
//	}
//}
//event.Entity.Find<iw::Tile>()->m_sprite.Colors32()[max] =
//	iw::Color(1, 0, 0, 1).to32();
//event.Entity.Find<iw::Tile>()->m_sprite.Colors32()[min] =
//	iw::Color(0, 1, 0, 1).to32();
//event.Entity.Find<iw::Tile>()->m_sprite.Colors32()[event.Entity.Find<iw::Tile>()->m_sprite.ColorCount32() - 1] =
//	iw::Color(1, 0, 0, 1).to32();
//event.Entity.Find<iw::Tile>()->m_sprite.Colors32()[0] =
//	iw::Color(0, 1, 0, 1).to32();
//split.Find<iw::Tile>()->m_sprite.Colors32()[split.Find<iw::Tile>()->m_sprite.ColorCount32() - 1] = 
//	iw::Color(1, 0, 0, 1).to32();
//split.Find<iw::Tile>()->m_sprite.Colors32()[0] =
//	iw::Color(0, 1, 0, 1).to32();
