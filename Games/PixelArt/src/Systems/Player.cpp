#include "Player.h"

int PlayerSystem::Initialize()
{
	PlayerEntity = Space->CreateEntity<
		iw::Transform,
		iw::Mesh,
		iw::Hull2,
		iw::Rigidbody,
		iw::Timer,
		Player>();

	iw::Transform*  t = PlayerEntity.Set<iw::Transform>(glm::vec3(20, 25, 0), glm::vec3(1, 2, 1));
	iw::Mesh*       m = PlayerEntity.Set<iw::Mesh>(m_playerMesh);
	iw::Hull2*      c = PlayerEntity.Set<iw::Hull2>(iw::MakeSquareCollider());
	iw::Rigidbody*  r = PlayerEntity.Set<iw::Rigidbody>();
	Player*         p = PlayerEntity.Set<Player>();

	iw::Timer* timer = PlayerEntity.Set<iw::Timer>();
	timer->SetTime("Light Attack", .25f);
	timer->SetTime("Heavy Attack", .5f);
	timer->SetTime("Heavy Attack Charge", 2.f);
	timer->SetTime("reset", 1.0f);

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
			p->OnGround |= true; // this dosnt work
		}

		/*else {
			p->OnGround = false;
		}*/
	};

	Physics->AddRigidbody(r);

	m->Material->Set("color", iw::Color::From255(240, 100, 100));

	return 0;
}

void PlayerSystem::Update()
{
	Space->Query<iw::Transform, iw::Rigidbody, iw::Timer, Player>().Each([&](
		iw::EntityHandle entity, 
		iw::Transform* transform,
		iw::Rigidbody* rigidbody,
		iw::Timer* timer,
		Player* player) 
	{
		timer->Tick();

		player->i_jump  = iw::Keyboard::KeyDown(iw::SPACE);
		player->i_up    = iw::Keyboard::KeyDown(iw::W);
		player->i_down  = iw::Keyboard::KeyDown(iw::S);
		player->i_left  = iw::Keyboard::KeyDown(iw::A);
		player->i_right = iw::Keyboard::KeyDown(iw::D);
		player->i_light = iw::Mouse::ButtonDown(iw::LMOUSE);
		player->i_heavy = iw::Mouse::ButtonDown(iw::RMOUSE);

		if (player->i_heavy) {
			glm::vec2 pos(transform->WorldPosition());
			glm::vec2 forward = pos + glm::vec2(player->Facing * 5, -5);
			m_coliderSystem->CutWorld(pos, forward);
		}

		if (iw::Keyboard::KeyDown(iw::Y)) {
			player->i_light = true;
			player->i_down  = true;
		}

		if (iw::Keyboard::KeyDown(iw::U)) {
			player->i_light = true;
			player->i_up = true;
		}

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
					else if (player->i_down) attack = AttackType::GROUND_LIGHT_DOWN;
					else                     attack = AttackType::GROUND_LIGHT_FORWARD;
				}

				else
				{
					     if (player->i_up)   attack = AttackType::GROUND_HEAVY_UP;
					else if (player->i_down) attack = AttackType::GROUND_HEAVY_DOWN;
					else                     attack = AttackType::GROUND_HEAVY_FORWARD;
				}
			}

			else {
				if (light)
				{
					     if (player->i_up)   attack = AttackType::AIR_LIGHT_UP;
					else if (player->i_down) attack = AttackType::AIR_LIGHT_DOWN;
					else                     attack = AttackType::AIR_LIGHT_FORWARD;
				}

				else
				{
					     if (player->i_up)   attack = AttackType::AIR_HEAVY_UP;
					else if (player->i_down) attack = AttackType::AIR_HEAVY_DOWN;
					else                     attack = AttackType::AIR_HEAVY_FORWARD;
				}
			}

			Bus->push<event_Attack>(attack, entity, player->Facing);
		}
	});
}
