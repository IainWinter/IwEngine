#include "Systems/Player_System.h"

int PlayerSystem::Initialize()
{
	m_player = MakeTile(*A_texture_player, component_list<Player, CorePixels, KeepInWorld, iw::Circle>());
	AddEntityToPhysics(m_player, Physics);

	iw::Rigidbody& rigidbody = m_player.get<iw::Rigidbody>();
	iw::Transform& transform = m_player.get<iw::Transform>();
	CorePixels&    core      = m_player.get<CorePixels>();
	Player&        player    = m_player.get<Player>();
	auto [w, h] = sand->GetSandTexSize2();
	transform.Position = glm::vec3(w, h, 0);
	rigidbody.SetTransform(&transform);
	rigidbody.SetMass(10);
	core.TimeWithoutCore = 4.f;
	player.SpecialLaser = MakeFatLaser_Cannon();
	
	player.Moves[0] = new Dash_Move();
	player.Moves[1] = new SmokeScreen_Move();
	player.Moves[2] = new EnergyShield_Move();

	Bus->push<ChangePlayerWeapon_Event>(WeaponType::DEFAULT_CANNON);
	Bus->send<CreatedPlayer_Event>(m_player); // set immediately in other systems before System Update
	Bus->send<CreatedCoreTile_Event>(m_player);

	m_handle = Console->AddHandler([&](
		const iw::Command& command)
	{
		Player& player = m_player.get<Player>();

		int active = command.Active ? 1 : -1;
			
		     if (command.Verb == "right")    player.i_moveX += active;
		else if (command.Verb == "up")       player.i_moveY += active;

		else if (command.Verb == "fire")     player.i_fire1 = command.Active;
		else if (command.Verb == "alt-fire") player.i_fire2 = command.Active;

		else if (command.Verb == "a")        player.Use[0] = command.Active;
		else if (command.Verb == "b")        player.Use[1] = command.Active;
		else if (command.Verb == "x")        player.Use[2] = command.Active;
		else if (command.Verb == "y")        player.Use[3] = command.Active;

		return false;
	});

	return 0;
}

void PlayerSystem::Destroy()
{
	if (m_player.is_alive())
	{
		m_player.destroy();
	}

	Console->RemoveHandler(m_handle);
}

// dont take inputs in fixed update 
void PlayerSystem::FixedUpdate()
{
	//glm::vec3& imp = player->imp;
	//glm::vec3& acc = rigidbody->Velocity;
	//glm::vec3 nacc = iw::norm(acc);
	//acc *= .95f;
	//if (glm::length(move.x) > 0.f) acc.x = player->CalcImp(move.x);
	//if (glm::length(move.y) > 0.f) acc.y = player->CalcImp(move.y);

	////acc.x = player->CalcAcc(acc.x, nacc.x, move.x, iw::FixedTime());
	////acc.y = player->CalcAcc(acc.y, nacc.y, move.y, iw::FixedTime());

	////glm::vec3 vel = /*imp + */acc;
	////LOG_INFO << vel.x;

	//float speed = glm::length(acc);
	//rigidbody->Velocity = iw::norm(acc) * glm::clamp(speed, 0.f, player->u_speed);

	//if (speeds.size() > 400)
	//{
	//	speeds.erase(speeds.begin());
	//}

	//speeds.push_back(speed);
}

void PlayerSystem::Update()
{
	timer.Tick();

	Player&        player    = m_player.get<Player>();
	iw::Rigidbody& rigidbody = m_player.get<iw::Rigidbody>();

	// movement

	player.move = glm::vec3(iw::norm(glm::vec2(player.i_moveX, player.i_moveY)), 0.f);
	rigidbody.Velocity = player.move * player.u_speed;

	// special actions

	for (int i = 0; i < 4; i++)
	{
		if (player.Moves[i])
		{
			player.Moves[i]->Update(APP_VARS_LIST, m_player, player.Use[i]);
		}
	}

	// weapons

	float aim_x = sand->sP.x;
	float aim_y = sand->sP.y;

	if (   player.i_fire1
		&& player.CurrentWeapon->CanFire())
	{
		ShotInfo shot = player.CurrentWeapon->GetShot(m_player, aim_x, aim_y);
		Bus->push<SpawnProjectile_Event>(shot);
		Bus->push<PlaySound_Event>(player.CurrentWeapon->Audio);

		if (player.CurrentWeapon->Ammo == 0)
		{
			Bus->push<ChangePlayerWeapon_Event>(WeaponType::DEFAULT_CANNON);
		}
	}
	
	if (   player.i_fire2 
		&& player.SpecialLaser->CanFire()
		&& player.can_fire_laser)
	{
		ShotInfo shot = player.SpecialLaser->GetShot(m_player, aim_x, aim_y);
		Bus->push<SpawnProjectile_Event>(shot);
		Bus->push<ChangeLaserFluid_Event>(-1);
	}

	// if dash equipped, and pressed button

	

	//// should warm when half of peices are gone, start a 5 second timer then u die
	//// if you die, shoud explode the player

	//if (player->CoreIndiceCount < player->CoreIndices.size() / 3) {
	//	Bus->push<EndGame_Event>();
	//}
}

void PlayerSystem::ImGui()
{
	//ImGui::Begin("ad");

	//ImGui::PlotLines("", speeds.data(), speeds.size(), 0, nullptr, 0, 250, ImVec2(360, 120), 4);

	//ImGui::End();
}

//todo: full power move if you have full health

bool PlayerSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case HEAL_PLAYER: 
		{
			HealPlayer_Event& event = e.as<HealPlayer_Event>();

			CorePixels& core = m_player.get<CorePixels>();
			iw::Tile&   tile = m_player.get<iw::Tile>();

			std::vector<int> candidates;

			if (event.IsCore)
			{
				for (const int& index : core.Indices)
				{
					if (core.ActiveIndices.find(index) == core.ActiveIndices.end())
					{
						candidates.push_back(index);
					}
				}
			}

			else 
			{
				for (const int& index : tile.m_removedCells)
				{
					if (core.Indices.find(index) == core.Indices.end())
					{
						candidates.push_back(index);
					}
				}
			}

			if (candidates.size() > 0)
			{
				float minDist = FLT_MAX;
				int healIndex = 0;

				for (const int& index : candidates)
				{
					int x = index % tile.m_sprite.m_width;
					int y = index / tile.m_sprite.m_width;
					int dx = core.CenterX - x;
					int dy = core.CenterY - y;
					float dist = sqrt(dx*dx+dy*dy);
						
					if (dist < minDist)
					{
						minDist = dist;
						healIndex = index;
					}
				}

				tile.ReinstatePixel(healIndex);

				if (event.IsCore)
				{
					core.ActiveIndices.insert(healIndex);
				}
			}

			break;
		}
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
			if (   timer.Can("damage sound")
				&& event.Config.Hit.has<Player>()
				&& event.Config.Projectile.is_alive())
			{
				// this should be the max speed of any bullet
				float impulse = glm::length(event.Config.Projectile.get<iw::Rigidbody>().Velocity);
				impulse = iw::clamp(impulse / 700, 0.f, 1.f);

				Bus->push<PlaySound_Event>("event:/impacts/player_hit", impulse);
			}
			break;
		}
		case CHANGE_PLAYER_WEAPON:
		{
			ChangePlayerWeapon_Event& event = e.as<ChangePlayerWeapon_Event>();
			Player& player = m_player.get<Player>();
			
			delete player.CurrentWeapon;
			
			switch (event.Type)
			{
				case WeaponType::DEFAULT_CANNON:      player.CurrentWeapon = MakeDefault_Cannon();     break;
				case WeaponType::MINIGUN_CANNON:      player.CurrentWeapon = MakeMinigun_Cannon();     break;
				case WeaponType::SPECIAL_BEAM_CANNON: player.CurrentWeapon = MakeSpecialBeam_Cannon(); break;
				case WeaponType::WATTZ_CANNON:        player.CurrentWeapon = MakeWattz_Cannon();       break;
				case WeaponType::BOLTZ_CANNON:        player.CurrentWeapon = MakeBoltz_Cannon();       break;
			}

			event.CurrentWeapon = player.CurrentWeapon; // pass onto Game_UI_Layer

			break;
		}
		case CORE_EXPLODED:
		{
			CoreExploded_Event& event = e.as<CoreExploded_Event>();
			if (event.Entity.has<Player>())
			{
				Task->delay(.2f, [=]() { Bus->push<iw::ActionEvent>(iw::ActionEventType::SINGLE, DESTROYED_PLAYER); });
				entities_defer().destroy(event.Entity);
			}

			break;
		}
	}

	return false;
}
