#include "Systems/Player.h"

int PlayerSystem::Initialize()
{
	player = sand->MakeTile("textures/SpaceGame/space-ship.png", true);
	AddComponentToPhysicsEntity<Player>(player);

	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	r->Transform.Scale = glm::vec3(.5);
	r->SetMass(10);

	return 0;
}

void PlayerSystem::FixedUpdate() 
{
	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	iw::Transform& t = r->Transform;

	glm::vec3 up  = -t.Up();
	glm::vec3 right = glm::vec3(-up.y, up.x, 0);
	glm::vec3 left = -right;

	bool i_thrust = iw::Keyboard::KeyDown(iw::LSHIFT);
	bool i_boost  = iw::Keyboard::KeyDown(iw::SPACE);
	bool i_left   = iw::Keyboard::KeyDown(iw::A);
	bool i_right  = iw::Keyboard::KeyDown(iw::D);

	// debugging

	//r->Velocity.x = 0;
	//r->Velocity.y = 0;

	//if (i_left)   r->Velocity.x = -150;
	//if (i_right)  r->Velocity.x =  150;
	//if (i_thrust) r->Velocity.y = -150;
	//if (i_boost)  r->Velocity.y =  150;

	//return;

	auto addThrust = [&](glm::vec3 offset, int dir, float newtons) 
	{
		glm::vec3 thrustPos = t.Position
			- up     * (32.f / sand->m_cellsPerMeter) 
			+ offset * (8.f  / sand->m_cellsPerMeter);

		r->ApplyForce(up * newtons);
		r->ApplyTorque(glm::vec3(0, 0, .33 * -dir));

		glm::vec2 cellPos = t.Position + -up * 32.f + offset * 8.f;

		iw::Cell fire = iw::Cell::GetDefault(iw::CellType::ROCK);

		sand->m_world->SetCell(cellPos.x, cellPos.y, fire);
	};

	float thrust = i_boost ? 200 : 50;

	if (i_thrust)
	{
		addThrust(left, -1, thrust);
		addThrust(right, 1, thrust);
	}

	if (i_left)
	{
		addThrust(left, -1, thrust);
	}

	if (i_right) {
		addThrust(right, 1, thrust);
	}
}

void PlayerSystem::Update()
{
	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	iw::Transform& t = r->Transform;

	cam_x = iw::lerp(cam_x, t.Position.x, iw::DeltaTime() * 5);
	cam_y = iw::lerp(cam_y, t.Position.y, iw::DeltaTime() * 5);

	sand->SetCamera(cam_x / 10, cam_y / 10, 10, 10);
}
