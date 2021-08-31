#include "Systems/CorePixels_System.h"

void CorePixelsSystem::Update()
{
	Space->Query<CorePixels>().Each(
		[&](
			iw::EntityHandle handle,
			CorePixels* core) 
		{
			bool dead = false;

			if (core->ActiveCount <= 0) {
				dead = true;
			}

			else if (core->ActiveCount < core->Indices.size() / 2) {
				core->Timer += iw::DeltaTime();
				if (core->Timer > core->TimeWithoutCore) {
					dead = true;
				}
			}

			else {
				core->Timer = iw::lerp(core->Timer, 0.f, iw::DeltaTime() * 10);
			}

			if (dead) {
				if (Space->HasComponent<Player>(handle)) {
					Bus->push<EndGame_Event>();
				}

				else {
					Space->QueueEntity(handle, iw::func_Destroy);
				}
			}
		});
}

bool CorePixelsSystem::On(iw::EntityCreatedEvent& e)
{
	if (   Space->HasComponent<iw::Tile>  (e.Entity)
		&& Space->HasComponent<CorePixels>(e.Entity))
	{
		iw::Tile*   tile = Space->FindComponent<iw::Tile>  (e.Entity);
		CorePixels* core = Space->FindComponent<CorePixels>(e.Entity);

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
		core->ActiveCount = core->Indices.size();
		core->CenterX /= core->ActiveCount;
		core->CenterY /= core->ActiveCount;
	}

	return false;
}

bool CorePixelsSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case PROJ_HIT_TILE:
		{
			auto [x, y, info, hit_, proj] = e.as<ProjHitTile_Event>().Config;
			int index = info.index;
			iw::Entity hit = hit_;

			// maybe the health doesnt stop moving and can go outside the screen
			// then the enemies could drop the shards and you could have to rush
			// them to get health...
			// if not this should go in the player system as it only effects them

			if (!hit.Has<Player>() || !hit.Has<CorePixels>()) break;

			CorePixels* core = hit.Find<CorePixels>();

			if (core->Indices.find(index) != core->Indices.end())
			{
				iw::Transform* transform = hit.Find<iw::Transform>();

				SpawnItem_Config config;
				config.X = transform->Position.x;
				config.Y = transform->Position.y;
				config.Item = PLAYER_CORE;
				config.ActivateDelay = .33f;
				config.Speed = 225;
				config.AngularSpeed = 10;
				config.OnPickup = [=]()
				{
					Bus->push<HealPlayer_Event>(index);
					hit.Find<CorePixels>()->Timer -= .5f;
				};

				Bus->push<SpawnItem_Event>(config);

				core->ActiveCount--;

				LOG_INFO << core->ActiveCount;
			}

			break;
		}
	}

	return false;
}
