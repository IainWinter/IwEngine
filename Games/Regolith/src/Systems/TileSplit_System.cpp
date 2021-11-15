#include "Systems/TileSplit_System.h"

bool TileSplitSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case REMOVE_CELL_FROM_TILE:
		{
			RemoveCellFromTile_Event& event = e.as<RemoveCellFromTile_Event>();

			iw::Tile* tile = event.Entity.Find<iw::Tile>();

			// Remove cells that are no longer connected to core

			size_t width  = tile->m_sprite.m_width;
			size_t height = tile->m_sprite.m_height;
			std::vector<cell_state> states = GetTileStates(tile);

			std::unordered_set<int> indices;
			if (event.Entity.Has<CorePixels>())
			{
				CorePixels* core = event.Entity.Find<CorePixels>();
			
				for (const int& seed : core->ActiveIndices)
				{
					flood_fill(seed, width, height, states, 
						[&indices](int index) {
							indices.insert(index);
						}
					);
				}

				// remove cells active in tile not in indices

				std::vector<int> toSplit;
				for (int i = 0; i < tile->m_currentCells.size(); i++)
				{
					int index = tile->m_currentCells.at(i);

					if (   indices            .find(index) == indices            .end()
						&& core->ActiveIndices.find(index) == core->ActiveIndices.end())
					{
						toSplit.push_back(index);
					}
				}
			
				if (toSplit.size() > 0) // not really needed but makes flow clear
				{
					SplitTileOff(event.Entity, toSplit);

					for (int i : toSplit)
					{
						tile->RemovePixel(i);
					}
				}
			}

			else
			{
				std::vector<std::vector<int>> islands;

				for (const int& seed : tile->m_currentCells)
				{
					std::vector<int>& island = islands.emplace_back();

					flood_fill(seed, width, height, states, 
						[&island](int index) {
							island.push_back(index);
						}
					);

					// remove empty islands
					if (island.size() == 0)
					{
						islands.pop_back();
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

					bool justCut = maxSize * 2 > tile->m_currentCells.size();

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
						event.Entity.Queue(iw::func_Destroy);
					}

					for (const std::vector<int>& island : islands)
					{
						SplitTileOff(event.Entity, island);

						// remove pixels from tile if just cutting off islands
						if (justCut)
						{
							for (int i : island)
							{
								tile->RemovePixel(i);
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

			if (tile->m_currentCells.size() < 10)
			{
				SplitTileOff(event.Entity, tile->m_currentCells);
				event.Entity.Queue(iw::func_Destroy);
			}

			break;
		}
	}

	return false;
}

void TileSplitSystem::SplitTileOff(
	iw::Entity entity, 
	std::vector<int> toSplit)
{
	iw::Tile* tile = entity.Find<iw::Tile>();
	glm::vec2 midOld = entity.Find<iw::Tile>()->m_sprite.Dimensions() / 2.f;

	if (toSplit.size() > 10)
	{
		// min index of split (bottom left corner in tile)

		int x = 0, y = 0; // might be min x max y?
		int min = INT_MAX, max = 0;
		for (int index : toSplit)
		{
			auto [px, py] = iw::xy<int>(index, tile->m_sprite.m_width);
			if (x < px) x = px;
			if (y < py) y = py;

			if (index > max) max = index;
			if (index < min) min = index;
		}

 		iw::Entity split = sand->SplitTile(entity, toSplit);

		// min of new - mid of old - mid of new
		// then rotate point into correct reference frame

		glm::vec2 midNew = split .Find<iw::Tile>()->m_sprite.Dimensions() / 2.f;
		glm::vec2 offset = glm::vec2(
			x - floor(midOld.x) - floor(midNew.x),
			y - floor(midOld.y) - floor(midNew.y));

		iw::Rigidbody* r = split.Find<iw::Rigidbody>();

		iw::Transform rotationNScale;
		rotationNScale.Rotation = r->Transform.WorldRotation();
		rotationNScale.Scale    = r->Transform.WorldScale();

		offset = iw::TransformPoint<iw::d2>(glm::vec2(offset), &rotationNScale);

		glm::vec3 o3 = glm::vec3(offset, 0.f);

		r->Transform.Position += o3;
		r->Velocity = o3; // could add vel of projectile
		r->AngularVelocity.z = iw::randf();

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
	}

	else
	{
		// make sparks the color of the cells
		for (int index : toSplit)
		{
			auto [x, y] = iw::xy<int>(index, tile->m_sprite.m_width);

			glm::vec2 offset = glm::vec2(
				x - floor(midOld.x),
				y - floor(midOld.y));

			glm::vec2 pos = iw::TransformPoint<iw::d2>(
				offset,
				entity.Find<iw::Transform>());

			iw::Cell spark;
			spark.Type  = iw::CellType::ROCK;
			spark.Props = iw::CellProp::MOVE_FORCE;
				
			spark.Color = iw::Color::From32(tile->m_sprite.Colors32()[index]);
				
			spark.dx = offset.x + iw::randfs() * 50;
			spark.dy = offset.y + iw::randfs() * 50;
			spark.life = .2 + iw::randf() * .2;

			float px = pos.x;
			float py = pos.y;
				
			sand->m_world->SetCell(px, py, spark);
		}
	}
}
