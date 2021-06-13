#include "Player.h"

int PlayerSystem::Initialize()
{
	iw::Entity player = Space->CreateEntity<
		iw::Transform,
		iw::Mesh,
		iw::Hull2,
		iw::Rigidbody,
		iw::Timer,
		Player>();

	iw::Transform*  t = player.Set<iw::Transform>(glm::vec3(20, 25, 0), glm::vec3(1, 2, 1));
	iw::Mesh*       m = player.Set<iw::Mesh>(m_playerMesh);
	iw::Hull2*      c = player.Set<iw::Hull2>(iw::MakeSquareCollider());
	iw::Rigidbody*  r = player.Set<iw::Rigidbody>();
	Player*         p = player.Set<Player>();

	iw::Timer* timer = player.Set<iw::Timer>();
	timer->SetTime("Attack", .25f);
	timer->SetTime("Attack 2", .5f);
	timer->SetTime("Attack 2 Charge", 2.f);

	r->Collider = c;
	r->SetTransform(t);
	r->IsAxisLocked.z = true;
	r->DynamicFriction = 0;
	r->StaticFriction= 0;

	r->OnCollision = [=](
		iw::Manifold& man, 
		float dt)
	{
		if (glm::dot(man.Normal, glm::vec3(0, 1, 0)) > 0)
		{
			p->OnGround = true;
		}

		else {
			p->OnGround = false;
		}
	};

	Physics->AddRigidbody(r);

	m->Material()->Set("albedo", iw::Color::From255(240, 100, 100));
}

void PlayerSystem::Update()
{
	Bus->push<event_Attack>(AttackType::GROUND_LIGHT_FORWARD);

	Space->Query<iw::Transform, iw::Rigidbody, iw::Timer, Player>().Each([&](
		iw::EntityHandle entity, 
		iw::Transform* transform,
		iw::Rigidbody* rigidbody,
		iw::Timer* timer,
		Player* player) 
	{
		player->i_jump  = iw::Keyboard::KeyDown(iw::SPACE);
		player->i_up    = iw::Keyboard::KeyDown(iw::W);
		player->i_down  = iw::Keyboard::KeyDown(iw::S);
		player->i_left  = iw::Keyboard::KeyDown(iw::A);
		player->i_right = iw::Keyboard::KeyDown(iw::D);
		player->i_light = iw::Mouse::ButtonDown(iw::LMOUSE);
		player->i_heavy = iw::Mouse::ButtonDown(iw::RMOUSE);

		// Movement

		float xVel = 0;

		if (player->i_left)  xVel -= 10;
		if (player->i_right) xVel += 10;

		if (player->OnGround)
		{
			if (player->i_jump) {
				player->OnGround = false;
				rigidbody->Velocity.y = 40;
			}

			if (xVel == 0) rigidbody->Velocity.x *= .9;
			else           rigidbody->Velocity.x = xVel;
		}

		else
		{
			if (xVel != 0) rigidbody->Velocity.x = xVel; // Cant stop in air?
		}

		if (abs(xVel) > 0) {
			player->Facing = signbit(xVel) ? -1 : 1;
		}

		// Attacks

		// Light attack

		AttackType attack;

		bool light = player->i_light && timer->Can("Light Attack");
		bool heavy = player->i_heavy && timer->Can("Heavy Attack");

		if (light || heavy)
		{
			if (player->OnGround)
			{
				if (light)
				{
					if (player->i_up)   attack = AttackType::GROUND_LIGHT_UP;
					if (player->i_down) attack = AttackType::GROUND_LIGHT_DOWN;
					else                attack = AttackType::GROUND_LIGHT_FORWARD;
				}

				else if (heavy)
				{
					if (player->i_up)   attack = AttackType::GROUND_HEAVY_UP;
					if (player->i_down) attack = AttackType::GROUND_HEAVY_DOWN;
					else                attack = AttackType::GROUND_HEAVY_FORWARD;
				}
			}

			else {
				if (light)
				{
					if (player->i_up)   attack = AttackType::AIR_LIGHT_UP;
					if (player->i_down) attack = AttackType::AIR_LIGHT_DOWN;
					else                attack = AttackType::AIR_LIGHT_FORWARD;
				}

				else if (heavy)
				{
					if (player->i_up)   attack = AttackType::AIR_HEAVY_UP;
					if (player->i_down) attack = AttackType::AIR_HEAVY_DOWN;
					else                attack = AttackType::AIR_HEAVY_FORWARD;
				}
			}

			Bus->push<event_Attack>(attack, entity);
		}
	});
}
