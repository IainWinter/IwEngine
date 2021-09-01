#include "Systems/Player_System.h"

void PlayerSystem::FixedUpdate()
{
	Player*        player    = m_player.Find<Player>();
	iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();

	rigidbody->Velocity.x = 0;
	rigidbody->Velocity.y = 0;

	if (player->i_up)    rigidbody->Velocity.y =  player->speed;
	if (player->i_down)  rigidbody->Velocity.y = -player->speed;
	if (player->i_right) rigidbody->Velocity.x =  player->speed;
	if (player->i_left)  rigidbody->Velocity.x = -player->speed;

	float angle = ((int)player->i_left - (int)player->i_right) * iw::Pi / 2;

	player->rotation = angle;// iw::lerp(player->rotation, angle, iw::FixedTime() * 5);
	rigidbody->Transform.Rotation = glm::angleAxis(player->rotation, glm::vec3(0, 0, 1));
}

void PlayerSystem::Update()
{
	if (!m_player.Alive()) return;

	Player* player = m_player.Find<Player>();

	player->i_up    = iw::Keyboard::KeyDown(iw::W); // should use event loop
	player->i_down  = iw::Keyboard::KeyDown(iw::S);
	player->i_left  = iw::Keyboard::KeyDown(iw::A);
	player->i_right = iw::Keyboard::KeyDown(iw::D);
	player->i_fire1 = iw::Mouse::ButtonDown(iw::LMOUSE);
	player->i_fire2 = iw::Mouse::ButtonDown(iw::RMOUSE);

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

bool PlayerSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case HEAL_PLAYER: 
		{
			HealPlayer_Event& event = e.as<HealPlayer_Event>();

			CorePixels* core = m_player.Find<CorePixels>();
			iw::Tile*   tile = m_player.Find<iw::Tile>();

			if (event.Index == -1)
			{
				std::vector<int> nonCoreIndices;
				for (int index : tile->m_removedCells)
				{
					if (core->Indices.find(index) == core->Indices.end())
					{
						nonCoreIndices.push_back(index);
					}
				}

				if (nonCoreIndices.size() > 0) 
				{
					float minDist = FLT_MAX;
					int healIndex = 0;

					for (int index : nonCoreIndices)
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
				}
			}

			else 
			{
				tile->ReinstatePixel(event.Index);

				if (core->Indices.find(event.Index) != core->Indices.end())
				{
					core->ActiveIndices.insert(event.Index);
				}
			}

			break;
		}
		case CHANGE_PLAYER_WEAPON:
		{
			ChangePlayerWeapon_Event& event = e.as<ChangePlayerWeapon_Event>();
			Player* player = m_player.Find<Player>();
			
			delete player->CurrentWeapon;
			
			switch (event.Weapon)
			{
				case WeaponType::DEFAULT_CANNON:      player->CurrentWeapon = MakeDefault_Cannon();     break;
				case WeaponType::MINIGUN_CANNON:      player->CurrentWeapon = MakeMinigun_Cannon();     break;
				case WeaponType::SPECIAL_BEAM_CANNON: player->CurrentWeapon = MakeSpecialBeam_Cannon(); break;
			}

			break;
		}
		case END_GAME: {
			m_player.Kill();
			break;
		}
		case RUN_GAME: {
			if (m_player)
			{
				m_player.Revive();
				m_player.Destroy();
			}
	
			m_player = sand->MakeTile<iw::Circle, Player, CorePixels, KeepInWorld>(A_texture_player, true);

			Player*        player    = m_player.Set<Player>();
			CorePixels*    core      = m_player.Set<CorePixels>();
			iw::Circle*    collider  = m_player.Find<iw::Circle>();
			iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();
			iw::Transform* transform = m_player.Find<iw::Transform>();

			auto [w, h] = sand->GetSandTexSize2();
			transform->Position = glm::vec3(w, h, 0);
			rigidbody->SetTransform(transform);
			rigidbody->SetMass(10);

			collider->Radius = 4;

			core->TimeWithoutCore = 4.f;

			player->CurrentWeapon = MakeDefault_Cannon();
			player->SpecialLaser  = MakeFatLaser_Cannon();
			player->CurrentWeapon->Ammo = -1;

			Bus->push<CreatedPlayer_Event>(m_player);

			break;
		}
	}

	return false;
}
