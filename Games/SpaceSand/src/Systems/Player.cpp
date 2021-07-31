#include "Systems/Player.h"

int PlayerSystem::Initialize()
{
	player = sand->MakeTile("textures/SpaceGame/player.png", true);
	AddComponentToPhysicsEntity<Player>(player);

	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	r->SetMass(10);

	auto [w, h] = sand->GetSandTexSize2();
	sand->SetCamera(w, h, 10, 10);

	return 0;
}

void PlayerSystem::FixedUpdate() 
{
	Player*        p = player.Find<Player>();
	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	iw::Transform& t = r->Transform;

	glm::vec3 up  = -t.Up();
	glm::vec3 right = glm::vec3(-up.y, up.x, 0);
	glm::vec3 left = -right;

	p->i_thrust = iw::Keyboard::KeyDown(iw::LSHIFT);
	p->i_boost  = iw::Keyboard::KeyDown(iw::SPACE);
	p->i_left   = iw::Keyboard::KeyDown(iw::A);
	p->i_right  = iw::Keyboard::KeyDown(iw::D);
	p->i_fire1  = iw::Mouse::ButtonDown(iw::LMOUSE);

	// debugging

	r->Velocity.x = 0;
	r->Velocity.y = 0;

	if (p->i_left)   r->Velocity.x = -150;
	if (p->i_right)  r->Velocity.x =  150;
	if (p->i_thrust) r->Velocity.y = -150;
	if (p->i_boost)  r->Velocity.y =  150;

	//return;

	//auto addThrust = [&](glm::vec3 offset, int dir, float newtons) 
	//{
	//	glm::vec3 thrustPos = t.Position
	//		- up     * (32.f / sand->m_cellsPerMeter) 
	//		+ offset * (8.f  / sand->m_cellsPerMeter);

	//	r->ApplyForce(up * newtons);
	//	r->ApplyTorque(glm::vec3(0, 0, .33 * -dir));

	//	glm::vec2 cellPos = t.Position + -up * 32.f + offset * 8.f;

	//	iw::Cell fire = iw::Cell::GetDefault(iw::CellType::ROCK);

	//	sand->m_world->SetCell(cellPos.x, cellPos.y, fire);
	//};

	//float thrust = p->i_boost ? 200 : 50;

	//if (p->i_thrust)
	//{
	//	addThrust(left, -1, thrust);
	//	addThrust(right, 1, thrust);
	//}

	//if (p->i_left)
	//{
	//	addThrust(left, -1, thrust);
	//}

	//if (p->i_right) {
	//	addThrust(right, 1, thrust);
	//}
}

void PlayerSystem::Update()
{
	Player*        p = player.Find<Player>();
	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	iw::Transform& t = r->Transform;

	p->fire1_timer += iw::DeltaTime();

	if (p->i_fire1 && p->fire1_timer > p->fire1_time)
	{
		p->fire1_timer = 0;

		// get direction towards mouse

		float x = t.Position.x,
			  y = t.Position.y;

		float sx = sand->sP.x,
			  sy = sand->sP.y;

		float dx = sx - t.Position.x,
			  dy = sy - t.Position.y;

		float length = sqrt(dx*dx + dy*dy);
		
		float nx = dx / length,
			  ny = dy / length;

		dx *= 20.f / length;
		dy *= 20.f / length;

		//dx += iw::randf();
		//dy += iw::randf();

		guns->MakeBulletC(x + nx * 10, y + ny * 10, dx, dy, 2);
	}

	//cam_x = iw::lerp(cam_x, t.Position.x, iw::DeltaTime() * 5);
	//cam_y = iw::lerp(cam_y, t.Position.y, iw::DeltaTime() * 5);

	//sand->SetCamera(cam_x / 10, cam_y / 10, 10, 10);
}
