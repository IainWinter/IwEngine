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
		else if (command.Verb == "fire")     player->i_fire1 = command.Active;
		else if (command.Verb == "alt-fire") player->i_fire2 = command.Active;

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

	glm::vec3& vel = rigidbody->Velocity;
	glm::vec3 nvel = iw::norm(vel);
	glm::vec3 move = iw::norm(glm::vec3(player->i_moveX, player->i_moveY, 0.f));
	
	vel.x = player->CalcVel(vel.x, nvel.x, move.x, iw::FixedTime());
	vel.y = player->CalcVel(vel.y, nvel.y, move.y, iw::FixedTime());

	float speed = glm::length(vel);
	vel = iw::norm(vel) * glm::clamp(speed, 0.f, player->u_speed);

	//// if move is     0, subtract breaking acceleration
	//// if move is not 0, add acceleration

	//glm::vec2 acc = glm::vec2(
	//	move.x != 0.f ? player->u_acceleration : -player->m_breaking,
	//	move.y != 0.f ? player->u_acceleration : -player->m_breaking
	//);

	//vel += glm::vec3(acc, 0.f);



	//glm::vec3 target(0.f);
	//target.x = player->i_moveX;
	//target.y = player->i_moveY;
	//
	//if (target != glm::vec3(0.f))
	//{
	//	target = glm::normalize(target) * player->u_speed;
	//}

	//glm::vec2 accel(0.f);
	//accel.x = target.x == 0.f ? -player->m_breaking : player->u_acceleration;
	//accel.y = target.y == 0.f ? -player->m_breaking : player->u_acceleration;

	//float speed1 = glm::length2(vel);

	//vel.x = iw::lerp(vel.x, target.x, accel.x * iw::FixedTime());
	//vel.y = iw::lerp(vel.y, target.y, accel.y * iw::FixedTime());

	//float speed2 = glm::length2(vel);

	//if (speed2 - speed1 < 0.f)
	//{
	//	if (abs(rigidbody->Velocity.x) < 4.0f) rigidbody->Velocity.x = 0.f;
	//	if (abs(rigidbody->Velocity.y) < 4.0f) rigidbody->Velocity.y = 0.f;
	//}

	//if (player->i_moveX == 0.f && player->i_moveY == 0.f)
	//{
	//	rigidbody->Velocity *= .95f;
	//}

	//else
	//{
	//	glm::vec2 dir = glm::normalize(glm::vec2(player->i_moveX, player->i_moveY));

	//	if (dir.x != 0.f) rigidbody->Velocity.x = player->speed * dir.x;
	//	if (dir.y != 0.f) rigidbody->Velocity.y = player->speed * dir.y;
	//}

	//auto safeNorm = [](glm::vec3 v) 
	//{
	//	return glm::length2(v) > 0.00001
	//		? glm::normalize(v) 
	//		: glm::vec3(0);
	//};

	//glm::vec2 dir = glm::normalize(glm::vec2(player->i_moveX, player->i_moveY));

	//rigidbody->Velocity.x = player->speed * dir.x;
	//rigidbody->Velocity.y = player->speed * dir.y;

	//// Target force, make own funciton in iwmath, i guess a turn twoards
	//glm::vec2 nVel = safeNorm(rigidbody->Velocity);
	//glm::vec2 delta = (dir - nVel) * 20.f;

	//rigidbody->Velocity = safeNorm(rigidbody->Velocity + glm::vec3(delta, 0.f)) * player->speed;

	//glm::vec2 vel = glm::vec2(rigidbody->Velocity.x, rigidbody->Velocity.y);
	//glm::vec2 dir = glm::vec2(player->i_moveX, player->i_moveY);

	//if (player->i_moveX == 0.f && player->i_moveY == 0.f)
	//{
	//	rigidbody->Velocity *= .99;
	//}

	//else
	//{
	//	dir = glm::normalize(dir);

	//	if (glm::length2(vel) < .001 || glm::dot(vel, dir) <= 0)
	//	{
	//		rigidbody->Velocity.x += player->speed / 2 * player->i_moveX;
	//		rigidbody->Velocity.y += player->speed / 2 * player->i_moveY;

	//		for (int i = 0; i < 25; i++)
	//		{
	//			glm::vec2 pos = glm::vec2(rigidbody->Transform.Position.x, rigidbody->Transform.Position.y);
	//			pos -= dir * 15.f + glm::vec2(iw::randf(), iw::randf()) * 5.f; // 30 = radius

	//			iw::Cell c = iw::Cell::GetDefault(iw::CellType::ROCK);

	//			c.Props = iw::CellProp::MOVE_FORCE;

	//			c.x = pos.x;
	//			c.y = pos.y;
	//			c.dx = -dir.x * 40 - iw::randfs() * 20;
	//			c.dy = -dir.y * 40 - iw::randfs() * 20;
	//			c.life = 1 + iw::randf();

	//			sand->m_world->SetCell(c.x, c.y, c);
	//		}
	//	}

	//	else
	//	{
	//		rigidbody->Velocity = glm::vec3(iw::lerp(vel, dir * player->speed, iw::FixedTime() * 10), 0.f);

	//		//rigidbody->Velocity.x += player->speed / 10 * player->i_moveX;
	//		//rigidbody->Velocity.y += player->speed / 10 * player->i_moveY;
	//	}
	//}
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

		if (player->CurrentWeapon->Audio.size())
		{
			Audio->Play(player->CurrentWeapon->Audio);
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
	
					m_player = sand->MakeTile<iw::Circle, Player, CorePixels, KeepInWorld>(*A_texture_player, true);

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
