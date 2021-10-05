#include "Systems/CorePixels_System.h"
#include <array>

enum cell_state : char
{
	EMPTY,
	FILLED = 100,
	VISITED = 10
};

void flood_fill(
	int index,
	int size_x, int size_y,
	std::vector<cell_state>& cells,
	std::unordered_set<int>& out_indices)
{
	//size_t index = x + y * size_y;

	if (   index < 0 
		|| index >= size_x * size_y) 
	{
		return;
	}

	if (cells[index] == FILLED)
	{
		cells[index] = VISITED;
		out_indices.insert(index);

		bool natLeft  = index % size_x != 0;
		bool natRight = index % size_x != size_x - 1;

		if (natRight) flood_fill(index + 1,      size_x, size_y, cells, out_indices);
		if (natLeft)  flood_fill(index - 1,      size_x, size_y, cells, out_indices);
					  flood_fill(index + size_x, size_x, size_y, cells, out_indices);
					  flood_fill(index - size_x, size_x, size_y, cells, out_indices);
		
		// diags

		if (natRight) flood_fill(index + 1 + size_x, size_x, size_y, cells, out_indices);
		if (natRight) flood_fill(index + 1 - size_x, size_x, size_y, cells, out_indices);
		if (natLeft)  flood_fill(index - 1 + size_x, size_x, size_y, cells, out_indices);
		if (natLeft)  flood_fill(index - 1 - size_x, size_x, size_y, cells, out_indices);
	}
}

void CorePixelsSystem::Update()
{
	Space->Query<iw::Tile, CorePixels>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Tile* tile,
			CorePixels* core) 
		{
			bool dead = false;
			
			if (tile->m_currentCells.size() < 30)
			{
				dead = true;
			}

			else if (core->ActiveIndices.size() <= 2) {
				dead = true;
			}

			else if (core->ActiveIndices .size() < core->Indices.size() / 3 * 2)
			{
				core->Timer += iw::DeltaTime();
				if (core->Timer > core->TimeWithoutCore) {
					dead = true;
				}
			}

			else {
				core->Timer = iw::lerp(core->Timer, 0.f, iw::DeltaTime() * 10);
			}

			if (dead) {
				Bus->push<CoreExploded_Event>(iw::Entity(handle, Space));

				if (Space->HasComponent<Player>(handle)) {
					Console->QueueCommand("game-over");
				}

				Space->QueueEntity(handle, iw::func_Destroy);
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
		});
}

bool CorePixelsSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case CREATED_CORE_TILE:
		{
			CreatedCoreTile_Event& event = e.as<CreatedCoreTile_Event>();

			iw::Tile*   tile = event.TileEntity.Find<iw::Tile>();
			CorePixels* core = event.TileEntity.Find<CorePixels>();

			if (!tile || !core)
			{
				LOG_ERROR << "Created a null entity? " << event.TileEntity.Handle.Index;
				break;
			}

			for (int i = 0; i < tile->m_sprite.ColorCount(); i += 4)
			{
				unsigned char& alpha = tile->m_sprite.Colors()[i+3];
				if (   alpha < 255
					&& alpha > 175)
				{
					alpha = 255;
				
					int index = i / 4;
					core->Indices.insert(index);
					core->CenterX += index % tile->m_sprite.m_width;
					core->CenterY += index / tile->m_sprite.m_width;
				}
			}
			core->ActiveIndices = core->Indices;
			core->CenterX /= core->ActiveIndices.size();
			core->CenterY /= core->ActiveIndices.size();

			break;
		}
		case REMOVE_CELL_FROM_TILE:
		{
			RemoveCellFromTile_Event& event = e.as<RemoveCellFromTile_Event>();
			int        index  = event.Index;
			iw::Entity entity = event.Entity;

			if (!Space->GetEntity(event.Entity.Handle).Alive())
			{
				LOG_ERROR << "Tried to remove cell from dead entity";
				break;
			}

			// maybe the health doesnt stop moving and can go outside the screen
			// then the enemies could drop the shards and you could have to rush
			// them to get health...
			// if not this should go in the player system as it only effects them
			
			// Remove core peice

			// crash where this returns incorrect components
			// not sure what root cause it prob event delay...
			// or it could be in the entity system

			CorePixels* core = entity.Find<CorePixels>();
			iw::Tile*   tile = entity.Find<iw::Tile>();
			
			if (!core || !tile) break;

			if (core->ActiveIndices.find(index) != core->ActiveIndices.end())
			{
				core->ActiveIndices.erase(index);

				if (entity.Has<Player>())
				{
					iw::Transform* transform = entity.Find<iw::Transform>();

					SpawnItem_Config config;
					config.X = transform->Position.x;
					config.Y = transform->Position.y;
					config.Item = PLAYER_CORE;
					config.ActivateDelay = .33f;
					config.Speed = 225;
					config.AngularSpeed = 10;
					config.DieWithTime = false;
					config.OnPickup = [=]()
					{
						Bus->push<HealPlayer_Event>(true);
						core->Timer -= .5f;
					};

					Bus->push<SpawnItem_Event>(config);
				}
			}

			// Remove cells that are no longer connected to core

			size_t width  = tile->m_sprite.m_width;
			size_t height = tile->m_sprite.m_height;

			std::vector<cell_state> states;
			states.resize(width * height);

			for (const int& index : tile->m_currentCells)
			{
				states.at(index) = tile->State(index) == iw::Tile::FILLED 
					? cell_state::FILLED 
					: cell_state::EMPTY;
			}

			std::unordered_set<int> indices;
			for (const int& seed : core->ActiveIndices)
			{
				flood_fill(seed, width, height, states, indices);
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
			
			//if (toSplit.size() > 20)
			//{
 			//	m_sand->SplitTile(entity, toSplit);
			//}

			for (const int& i : toSplit)
			{
				tile->SetState(i, iw::Tile::PixelState::REMOVED);
			}

			break;
		}
	}

	return false;
}
