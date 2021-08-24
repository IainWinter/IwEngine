#include "Systems/Player_System.h"

int PlayerSystem::Initialize()
{
	m_player = sand->MakeTile<iw::Circle, Player>(A_texture_player, true);

	Player*        player    = m_player.Set<Player>();
	iw::Circle*    collider  = m_player.Find<iw::Circle>();
	iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();
	
	player->CurrentWeapon = MakeSpecialBeam_Cannon();
	player->SpecialLaser  = MakeFatLaser_Cannon();

	player->CurrentWeapon->Ammo = -1;

	//collider->Radius = 4;

	auto [w, h] = sand->GetSandTexSize2();
	rigidbody->Transform.Position = glm::vec3(w, h, 0);
	rigidbody->SetMass(10);

	rigidbody->OnCollision = [](auto man, float dt){
		LOG_INFO << "Player collision";
	};

	return 0;
}

void PlayerSystem::FixedUpdate()
{
	Player*        player    = m_player.Find<Player>();
	iw::Rigidbody* rigidbody = m_player.Find<iw::Rigidbody>();

	player->i_up    = iw::Keyboard::KeyDown(iw::W); // should use event loop
	player->i_down  = iw::Keyboard::KeyDown(iw::S);
	player->i_left  = iw::Keyboard::KeyDown(iw::A);
	player->i_right = iw::Keyboard::KeyDown(iw::D);
	player->i_fire1 = iw::Mouse::ButtonDown(iw::LMOUSE);
	player->i_fire2 = iw::Mouse::ButtonDown(iw::RMOUSE);

	rigidbody->Velocity.x = 0;
	rigidbody->Velocity.y = 0;

	int borderFar = 375;
	int borderNear = 25;

	bool atTop    = rigidbody->Transform.Position.y > borderFar;
	bool atBottom = rigidbody->Transform.Position.y < borderNear;
	bool atRight  = rigidbody->Transform.Position.x > borderFar;
	bool atLeft   = rigidbody->Transform.Position.x < borderNear;

	float speed = 150;

	if (player->i_up)    rigidbody->Velocity.y = atTop    ? 0 :  speed; // todo: make this slow stop
	if (player->i_down)  rigidbody->Velocity.y = atBottom ? 0 : -speed;
	if (player->i_right) rigidbody->Velocity.x = atRight  ? 0 :  speed;
	if (player->i_left)  rigidbody->Velocity.x = atLeft   ? 0 : -speed;
}

void PlayerSystem::Update()
{
	Player*        p = m_player.Find<Player>();
	iw::Transform* t = m_player.Find<iw::Transform>();

	float aim_x = sand->sP.x;
	float aim_y = sand->sP.y;

	if (   p->i_fire1
		&& p->CurrentWeapon->CanFire())
	{
		ShotInfo shot = p->CurrentWeapon->GetShot(m_player, aim_x, aim_y);
		Bus->push<SpawnProjectile_Event>(shot);

		if (p->CurrentWeapon->Ammo == 0)
		{
			Bus->push<ChangePlayerWeapon_Event>(WeaponType::DEFAULT_CANNON);
		}
	}
	
	if (   p->i_fire2 
		&& p->SpecialLaser->CanFire()
		&& p->can_fire_laser)
	{
		ShotInfo shot = p->SpecialLaser->GetShot(m_player, aim_x, aim_y);
		Bus->push<SpawnProjectile_Event>(shot);
		Bus->push<ChangeLaserFluid_Event>(-1);
	}
}

bool PlayerSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case HEAL_PLAYER: 
		{
			m_player.Find<iw::Tile>()->ReinstateRandomPixel();
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
	}

	return false;
}
