#include "Layers/Game_Layer.h"

int Game_Layer::Initialize()
{
	auto [sandWidth, sandHeight] = sand->GetSandTexSize2();
	sand->SetCamera(sandWidth, sandHeight);

	Renderer->Device->SetClearColor(0, 0, 0, 1);

	projectile_s = new ProjectileSystem (sand);
	item_s       = new ItemSystem       (sand);
	recorder_s   = new Recording_System (sand);
	tile_s       = new TileSplitSystem  (sand);

	PushSystem<PlayerLaserTankSystem>(sand_ui_laserCharge);
	PushSystem<PlayerSystem>         (sand);
	PushSystem<EnemySystem>          (sand);
	PushSystem<CorePixelsSystem>     (sand);
	PushSystem<ExplosionSystem>      (sand);
	PushSystem<WorldSystem>          (sand);
	PushSystem(tile_s);
	PushSystem(item_s);
	PushSystem(projectile_s);
	PushSystem(recorder_s);
	PushSystem<FlockingSystem>       ();
	PushSystem<KeepInWorldSystem>    ();
	PushSystem<Upgrade_System>       ();
	PushSystem<ScoreSystem>          ();

	Console->AddHandler(
		[=](const iw::Command& command)
		{
			if (command.Verb == "toolbox")
			{
				SpawnAsteroid_Config config;
				config.Size = 1;
				config.SpawnLocationX = sand->sP.x;
				config.SpawnLocationY = sand->sP.y;

				Bus->push<SpawnAsteroid_Event>(config);
			}

			return false;
		}
	);

	return Layer::Initialize();
}

bool Game_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case DESTROYED_PLAYER:
		{
			LOG_TRACE << "DESTROYED_PLAYER";

			std::vector<iw::ISystem*> systems = temp_GetSystems().m_items;

			for (iw::ISystem* s : systems)
			{
				if (   s == projectile_s // to keep projs moving for a second after death
					|| s == recorder_s)  // to grab from layer in App
				{
					continue;
				}

				PopSystem(s);
			}

			// This is only an est on the framerate, should record a few frames after death
			// below is the ratio of frames after death vs before

			float time = recorder_s->m_frameCount * .3f * iw::DeltaTime();

			recorder_s->m_freezeCamera = true;

			Task->delay(time,
				[=]() {
					recorder_s->m_record = false;
				}
			);

			Task->delay(time + 2.f,
				[=]() {
					// this sucks, should be able to just clear memory and not worry about .on_destroy events spawning new entities. 
					// Could block new entities being created, but then everything needs to check for that not returning a valid entity
					// Could block on_destroy, but that could be used to cleanup memory or in case of physics, remove the entities.
					// So I cannot really think of a better solution, but this only works if a destroied entity creates less entities than itself.
					// Could also block the event bus to stop explosion events for instance.

					// This could be the solve tho, should look into how others handle this. I assume the way is some form of freezing spawning new entities mixed with recursive deletion.

					bool entitiesStillExist = true;  
					while (entitiesStillExist)
					{
						entitiesStillExist = false;  
						for (auto [e] : entities().all())
						{
							entitiesStillExist = true;
							entities_defer().destroy(e);
						}
						entities_defer().execute();
					}
					
					entities().m_storage.clear();

					for (iw::ISystem* s : systems) // only because popping a system to pause it makes it so it cannot get deleted, need a third 'paused' state in EventStack, real solve is to rethink how this works
					{
						if (s == recorder_s) continue;
						DestroySystem(s);
					}

					std::stringstream ss; ss << "set-death-movie-frame " << recorder_s->m_frameToDraw + 1;
					Console->QueueCommand("set-state post");
					Console->QueueCommand(ss.str());
				}
			);

			// I think that the animation will look better in UI land instead of the sand
			// ahh do need to keep the sand alive for a little for the projectiles tho
			// for testing this simple verson, forget animations!!

			//PushSystem<DeathTransition_System>(sand, 3);

			break;
		}
	}

	return Layer::On(e);
}
