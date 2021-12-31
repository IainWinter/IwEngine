#include "Layers/Game_Layer.h"

int Game_Layer::Initialize()
{
	auto [sandWidth, sandHeight] = sand->GetSandTexSize2();
	sand->SetCamera(sandWidth, sandHeight);

	Renderer->Device->SetClearColor(0, 0, 0, 1);

	proj_s = new ProjectileSystem(sand);
	item_s = new ItemSystem      (sand);

	PushSystem<PlayerLaserTankSystem>(sand_ui_laserCharge);
	PushSystem<EnemySystem>          (sand);
	PushSystem<CorePixelsSystem>     (sand);
	PushSystem<TileSplitSystem>      (sand);
	PushSystem<ExplosionSystem>      (sand);
	PushSystem<WorldSystem>          (sand);
	PushSystem<PlayerSystem>         (sand);
	PushSystem(item_s);
	PushSystem(proj_s);
	PushSystem<FlockingSystem>       ();
	PushSystem<KeepInWorldSystem>    ();
	PushSystem<Upgrade_System>       ();
	PushSystem<ScoreSystem>          ();

	return Layer::Initialize();
}

void Game_Layer::Destroy()
{
	iw::ComponentQuery query = Space->MakeQuery();
	query.SetAny({
		Space->GetComponent<Player>(),
		Space->GetComponent<Enemy>(),
		Space->GetComponent<Asteroid>(),
		Space->GetComponent<Projectile>(),
		Space->GetComponent<Item>()
	});

	Space->Query(query).Each([&](iw::EntityHandle handle) {
		Space->QueueEntity(handle, iw::func_Destroy);
	});

	Layer::Destroy();
}

bool Game_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case DESTROIED_PLAYER:
		{
			LOG_TRACE << "DESTROIED_PLAYER";

			for(iw::ISystem* s : temp_GetSystems())
			{
				if (   s == proj_s
					/*|| s == item_s*/)
				{
					continue;
				}

				DestroySystem(s);
			}

			PushSystem<DeathTransition_System>(sand, 3);
			Console->QueueCommand("fade 3. 4.");

			float time = iw::RawTotalTime();
			Task->coroutine(
				[this, time]()
			{
				bool done = iw::RawTotalTime() - time > 7.f;
				if (done) Console->QueueCommand("set-state highscores");
				return done;
			});

			break;
		}
	}

	return Layer::On(e);
}
