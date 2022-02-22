#include "Layers/Game_Layer.h"

int Game_Layer::Initialize()
{
	auto [sandWidth, sandHeight] = sand->GetSandTexSize2();
	sand->SetCamera(sandWidth, sandHeight);

	Renderer->Device->SetClearColor(0, 0, 0, 1);

	projectile_s = new ProjectileSystem (sand);
	item_s       = new ItemSystem       (sand);
	recorder_s   = new Recording_System (sand);

	PushSystem<PlayerLaserTankSystem>(sand_ui_laserCharge);
	PushSystem<EnemySystem>          (sand);
	PushSystem<CorePixelsSystem>     (sand);
	PushSystem<TileSplitSystem>      (sand);
	PushSystem<ExplosionSystem>      (sand);
	PushSystem<WorldSystem>          (sand);
	PushSystem(item_s);
	PushSystem(projectile_s);
	PushSystem(recorder_s);
	PushSystem<FlockingSystem>       ();
	PushSystem<KeepInWorldSystem>    ();
	PushSystem<Upgrade_System>       ();
	PushSystem<ScoreSystem>          ();
	PushSystem<PlayerSystem>         (sand);

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

			for(iw::ISystem* s : temp_GetSystems())
			{
				if (   s == projectile_s // to keep projs moving for a second after death
					|| s == recorder_s)  // to grab from layer in App
				{
					continue;
				}

				DestroySystem(s);
			}

			Console->QueueCommand("set-state post");

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
