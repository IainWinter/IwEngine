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

void Game_Layer::Destroy()
{
	//iw::ComponentQuery query = Space->MakeQuery();
	//query.SetAny({
	//	Space->GetComponent<Player>(),
	//	Space->GetComponent<Enemy>(),
	//	Space->GetComponent<Asteroid>(),
	//	Space->GetComponent<Projectile>(),
	//	Space->GetComponent<Item>()
	//});

	//Space->Query(query).Each([&](iw::EntityHandle handle) {
	//	Space->QueueEntity(handle, iw::func_Destroy);
	//});

	Layer::Destroy();
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

			Task->delay(time,
				[=]() {
					recorder_s->m_record = false;
				}
			);

			Task->delay(time + 1.f,
				[=]() {
					for (auto [e] : entities().all())
					{
						entities_defer().destroy(e);
					}
					entities_defer().execute();
					//entities().m_storage.clear();

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
			//Console->QueueCommand("fade 3. 4.");

			//float time = iw::RawTotalTime();
			//Task->coroutine(
			//	[this, time]()
			//{
			//	bool done = iw::RawTotalTime() - time > 7.f;
			//	if (done) Console->QueueCommand("set-state post");
			//	return done;
			//});

			break;
		}
	}

	return Layer::On(e);
}
