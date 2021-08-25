#include "Systems/Player_System.h"

int PlayerSystem::Initialize()
{
	m_player = sand->MakeTile<iw::Circle, Player, KeepInWorld>(A_texture_player, true);

	Player*        player    = m_player.Set<Player>();
	iw::Circle*    collider  = m_player.Find<iw::Circle>();
	iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();
	iw::Tile*      tile      = m_player.Find<iw::Tile>();

	player->CurrentWeapon = MakeDefault_Cannon();
	player->SpecialLaser  = MakeFatLaser_Cannon();

	player->CurrentWeapon->Ammo = -1;

	collider->Radius = 4;

	auto [w, h] = sand->GetSandTexSize2();
	rigidbody->Transform.Position = glm::vec3(w, h, 0);
	rigidbody->SetMass(10);

	rigidbody->OnCollision = [](auto man, float dt){
		LOG_INFO << "Player collision";
	};

	player->CoreIndices = {
		34, 35, 36, 37,
		42, 43, 44, 45,
		50, 51,
		58, 59
	};

	for (unsigned index : player->CoreIndices)
	{
		tile->m_sprite.Colors32()[index] = iw::Color::From255(255, 100, 10).to32();
	}

	return 0;
}

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
}

void PlayerSystem::Update()
{
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
}

bool PlayerSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case HEAL_PLAYER: 
		{
			HealPlayer_Event& event = e.as<HealPlayer_Event>();

			if (event.Index == -1)
			{
				m_player.Find<iw::Tile>()->ReinstateRandomPixel();
			}

			else {
				m_player.Find<iw::Tile>()->ReinstatePixel(event.Index);
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
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();

			if (event.Hit.Handle == m_player.Handle) // this doesnt work without .Handle????
			{
				Player*        player    = m_player.Find<Player>();
				iw::Transform* transform = m_player.Find<iw::Transform>();

				if (iw::contains(player->CoreIndices, event.Info.index))
				{
					SpawnItem_Config config;
					config.X = transform->Position.x;
					config.Y = transform->Position.y;
					config.Item = PLAYER_CORE;
					config.ActivateDelay = .33f;
					config.Speed = 200;
					config.OnPickup = [=]()
					{
						Bus->push<HealPlayer_Event>(event.Info.index);
						Bus->push<HealPlayer_Event>(-1);
						Bus->push<HealPlayer_Event>(-1);
						Bus->push<HealPlayer_Event>(-1);
					};

					Bus->push<SpawnItem_Event>(config);
				}
			}

			break;
		}
	}

	return false;
}
