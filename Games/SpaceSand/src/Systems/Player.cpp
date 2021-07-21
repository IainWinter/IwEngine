#include "Systems/Player.h"

int PlayerSystem::Initialize()
{
	player = sand->MakeTile("textures/SpaceGame/space-ship.png", true);
	
	Physics->RemoveCollisionObject(player.Find<iw::Rigidbody>()); // this can be removed with entity cleanup
	player.Add<Player>();

	iw::Rigidbody* r = player.Find<iw::Rigidbody>();

	Physics->AddRigidbody(r);                       // this can be removed with entity cleanup
	r->Collider = player.Find<iw::MeshCollider2>(); // <
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

	auto addThrust = [&](glm::vec3 offset, int dir, float newtons) 
	{
		glm::vec3 thrustPos = t.Position
			- up     * (32.f / sand->m_cellsPerMeter) 
			+ offset * (8.f  / sand->m_cellsPerMeter);

		r->ApplyForce(up * newtons);
		r->ApplyTorque(glm::vec3(0, 0, .33 * -dir));

		glm::vec2 cellPos = t.Position + -up * 32.f + offset * 8.f;

		iw::Cell fire = iw::Cell::GetDefault(iw::CellType::FIRE);

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
