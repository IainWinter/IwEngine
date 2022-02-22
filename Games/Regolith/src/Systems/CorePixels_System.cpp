#include "Systems/CorePixels_System.h"

void CorePixelsSystem::Update()
{
	for (auto [entity, tile, core] : entities().query<iw::Tile, CorePixels>().with_entity())
	{
		bool dead = false;
			
		if (tile.m_currentCells.size() < 30)
		{
			dead = true;
		}

		else if (core.ActiveIndices.size() <= 2) {
			dead = true;
		}

		else if (core.ActiveIndices .size() < core.Indices.size() / 3 * 2)
		{
			core.Timer += iw::DeltaTime();
			if (core.Timer > core.TimeWithoutCore) {
				dead = true;
			}
		}

		else {
			core.Timer = iw::lerp(core.Timer, 0.f, iw::DeltaTime() * 10);
		}

		if (dead)
		{
			if (entity.has<Player>())
			{
				for (int index : core.ActiveIndices)
				{
					RemoveCorePixel(entity, index);
				}
			}

			Bus->push<CoreExploded_Event>(entity);

			// this needs to remove the behaviour components from the entity?
			//Space.QueueEntity(handle, iw::func_Destroy);
		}

		// if a core peice isnt touching a regular peice is dies
		//if (!Space->HasComponent<Player>(handle))
		//{
		//	for (const int& index : core->ActiveIndices)
		//	{
		//		int top   = index + tile->m_sprite.m_width; // doesnt work if core cells are on edge
		//		int bot   = index - tile->m_sprite.m_width;
		//		int right = index + 1;
		//		int left  = index - 1;

		//		auto isCorePeice = [&](int i)
		//		{
		//			return core->ActiveIndices.find(i) != core->ActiveIndices.end();
		//		};

		//		auto isRegularPeice = [&](int i)
		//		{
		//			return core->ActiveIndices.find(i) == core->ActiveIndices.end()
		//				&& tile->State(i) == iw::Tile::PixelState::FILLED;
		//		};

		//		int corePeices = (int)isCorePeice(top)
		//						  + (int)isCorePeice(bot)
		//						  + (int)isCorePeice(left)
		//						  + (int)isCorePeice(right);

		//		int regularPeices = (int)isRegularPeice(top)
		//						  + (int)isRegularPeice(bot)
		//						  + (int)isRegularPeice(left)
		//						  + (int)isRegularPeice(right);

		//		if (regularPeices <= 1 && corePeices < 2)
		//		{
		//			Bus->push<RemoveCellFromTile_Event>(index, iw::Entity(handle, Space));
		//		}
		//	}
		//}
	}
}

bool CorePixelsSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case CREATED_CORE_TILE:
		{
			CreatedCoreTile_Event& event = e.as<CreatedCoreTile_Event>();

			iw::Tile&   tile = event.TileEntity.get<iw::Tile>();
			CorePixels& core = event.TileEntity.get<CorePixels>();

			for (int i = 0; i < tile.m_sprite.ColorCount(); i += 4)
			{
				unsigned char& alpha = tile.m_sprite.Colors()[i+3];
				if (   alpha < 255
					&& alpha > 175)
				{
					alpha = 255;
				
					int index = i / 4;
					core.Indices.insert(index);
					core.CenterX += index % tile.m_sprite.m_width;
					core.CenterY += index / tile.m_sprite.m_width;
				}
			}
			core.ActiveIndices = core.Indices;
			core.CenterX /= core.ActiveIndices.size();
			core.CenterY /= core.ActiveIndices.size();

			break;
		}
		case REMOVE_CELL_FROM_TILE:
		{
			RemoveCellFromTile_Event& event = e.as<RemoveCellFromTile_Event>();
			
			if (event.Entity.has<CorePixels>())
			{
				RemoveCorePixel(event.Entity, event.Index);
			}

			break;
		}
	}

	return false;
}

void CorePixelsSystem::RemoveCorePixel(
	entity entity,
	int index)
{
	// maybe the health doesnt stop moving and can go outside the screen
	// then the enemies could drop the shards and you could have to rush
	// them to get health...
	// if not this should go in the player system as it only effects them
			
	CorePixels& core = entity.get<CorePixels>();
	iw::Tile&   tile = entity.get<iw::Tile>();
			
	if (core.ActiveIndices.find(index) != core.ActiveIndices.end())
	{
		core.ActiveIndices.erase(index);

		// see above comment
		if (entity.has<Player>())
		{
			iw::Transform& transform = entity.get<iw::Transform>();

			SpawnItem_Config config;
			config.X = transform.Position.x;
			config.Y = transform.Position.y;
			config.Item = ItemType::PLAYER_CORE;
			config.ActivateDelay = .33f;
			config.Speed = 225;
			config.AngularSpeed = 10;
			config.DieWithTime = false;
			config.OnPickup = [&]()
			{
				Bus->push<HealPlayer_Event>(true);
				core.Timer -= .5f;
			};

			Bus->push<SpawnItem_Event>(config);
		}
	}
}
