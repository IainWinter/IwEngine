#include "Systems/Player_System.h"

int PlayerSystem::Initialize()
{
	m_handle = Console->AddHandler([&](
		const iw::Command& command)
	{
		Player* player = m_player.Find<Player>();

		if (!player) return false;

		int active = command.Active ? 1 : -1;

		     if (command.Verb == "right")    player->i_moveX += active;
		else if (command.Verb == "up")       player->i_moveY += active;
		else if (command.Verb == "fire")     player->i_fire1  = command.Active;
		else if (command.Verb == "alt-fire") player->i_fire2  = command.Active;

		return false;
	});

	return 0;
}

void PlayerSystem::Destroy()
{
	Console->RemoveHandler(m_handle);
}

void PlayerSystem::FixedUpdate()
{
	Player*        player    = m_player.Find<Player>();
	iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();

	if (!player) return;

	rigidbody->Velocity.x = player->speed * player->i_moveX;
	rigidbody->Velocity.y = player->speed * player->i_moveY;

	//float angle = ((int)player->i_left - (int)player->i_right) * iw::Pi / 2;
	//player->rotation = iw::lerp(player->rotation, angle, iw::FixedTime() * 5);
	//rigidbody->Transform.Rotation = glm::angleAxis(player->rotation, glm::vec3(0, 0, 1));
}

void PlayerSystem::Update()
{
	Player* player = m_player.Find<Player>();

	if (!player) return;

	float aim_x = sand->sP.x;
	float aim_y = sand->sP.y;

	if (   player->i_fire1
		&& player->CurrentWeapon->CanFire())
	{
		ShotInfo shot = player->CurrentWeapon->GetShot(m_player, aim_x, aim_y);
		Bus->push<SpawnProjectile_Event>(shot);

		if (player->CurrentWeapon->Ammo == 0)
		{
			Bus->push<ChangePlayerWeapon_Event>(WeaponType::DEFAULT_CANNON);
		}
	}
	
	if (   player->i_fire2 
		&& player->SpecialLaser->CanFire()
		&& player->can_fire_laser)
	{
		ShotInfo shot = player->SpecialLaser->GetShot(m_player, aim_x, aim_y);
		Bus->push<SpawnProjectile_Event>(shot);
		Bus->push<ChangeLaserFluid_Event>(-1);
	}

	//// should warm when half of peices are gone, start a 5 second timer then u die
	//// if you die, shoud explode the player

	//if (player->CoreIndiceCount < player->CoreIndices.size() / 3) {
	//	Bus->push<EndGame_Event>();
	//}
}

//todo: full power move if you have full health

bool PlayerSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case HEAL_PLAYER: 
		{
			HealPlayer_Event& event = e.as<HealPlayer_Event>();

			CorePixels* core = m_player.Find<CorePixels>();
			iw::Tile*   tile = m_player.Find<iw::Tile>();

			std::vector<int> candidates;

			if (event.IsCore)
			{
				for (const int& index : core->Indices)
				{
					if (core->ActiveIndices.find(index) == core->ActiveIndices.end())
					{
						candidates.push_back(index);
					}
				}
			}

			else 
			{
				for (const int& index : tile->m_removedCells)
				{
					if (core->Indices.find(index) == core->Indices.end())
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
					int x = index % tile->m_sprite.m_width;
					int y = index / tile->m_sprite.m_width;
					int dx = core->CenterX - x;
					int dy = core->CenterY - y;
					float dist = sqrt(dx*dx+dy*dy);
						
					if (dist < minDist)
					{
						minDist = dist;
						healIndex = index;
					}
				}

				tile->ReinstatePixel(healIndex);

				if (event.IsCore)
				{
					core->ActiveIndices.insert(healIndex);
				}
			}

			break;
		}
		case CHANGE_PLAYER_WEAPON:
		{
			ChangePlayerWeapon_Event& event = e.as<ChangePlayerWeapon_Event>();
			Player* player = m_player.Find<Player>();
			
			delete player->CurrentWeapon;
			
			switch (event.Type)
			{
				case WeaponType::DEFAULT_CANNON:      player->CurrentWeapon = MakeDefault_Cannon();     break;
				case WeaponType::MINIGUN_CANNON:      player->CurrentWeapon = MakeMinigun_Cannon();     break;
				case WeaponType::SPECIAL_BEAM_CANNON: player->CurrentWeapon = MakeSpecialBeam_Cannon(); break;
				case WeaponType::WATTZ_CANNON:        player->CurrentWeapon = MakeWattz_Cannon();       break;
				case WeaponType::BOLTZ_CANNON:        player->CurrentWeapon = MakeBoltz_Cannon();       break;
			}

			event.CurrentWeapon = player->CurrentWeapon; // pass onto Game_UI_Layer

			break;
		}
		
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case GAME_START_STATE:
				{
					if (m_player)
					{
						m_player.Destroy();
					}
	
					m_player = sand->MakeTile<iw::Circle, Player, CorePixels, KeepInWorld>(A_texture_player, true);

					iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();
					iw::Transform* transform = m_player.Find<iw::Transform>();
					CorePixels*    core      = m_player.Set<CorePixels>();
					Player*        player    = m_player.Set<Player>();

					auto [w, h] = sand->GetSandTexSize2();
					transform->Position = glm::vec3(w, h, 0);

					rigidbody->SetTransform(transform);
					rigidbody->SetMass(10);

					core->TimeWithoutCore = 4.f;
					
					player->SpecialLaser = MakeFatLaser_Cannon();
					
					Bus->push<ChangePlayerWeapon_Event>(WeaponType::DEFAULT_CANNON);
					Bus->push<CreatedPlayer_Event>(m_player);
					Bus->push<CreatedCoreTile_Event>(m_player);

					break;
				}
			}

			break;
		}
	}

	return false;
}
