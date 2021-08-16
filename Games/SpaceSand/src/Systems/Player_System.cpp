#include "Systems/Player_System.h"

int PlayerSystem::Initialize()
{
	AddComponentToPhysicsEntity<Player>(player);

	auto [w, h] = sand->GetSandTexSize2();

	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	r->SetMass(10);

	r->Transform.Position = glm::vec3(w, h, 0);
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

	p->i_up    = iw::Keyboard::KeyDown(iw::W);
	p->i_down  = iw::Keyboard::KeyDown(iw::S);
	p->i_left  = iw::Keyboard::KeyDown(iw::A);
	p->i_right = iw::Keyboard::KeyDown(iw::D);
	p->i_fire1 = iw::Mouse::ButtonDown(iw::RMOUSE);

	r->Velocity.x = 0;
	r->Velocity.y = 0;

	int borderFar = 375;
	int borderNear = 25;

	bool atTop    = t.Position.y > borderFar;
	bool atBottom = t.Position.y < borderNear;
	bool atRight  = t.Position.x > borderFar;
	bool atLeft   = t.Position.x < borderNear;

	float speed = 150;

	if (p->i_up)    r->Velocity.y = atTop    ? 0 :  speed; // todo: make this slow stop
	if (p->i_down)  r->Velocity.y = atBottom ? 0 : -speed;
	if (p->i_right) r->Velocity.x = atRight  ? 0 :  speed;
	if (p->i_left)  r->Velocity.x = atLeft   ? 0 : -speed;
}

void PlayerSystem::Update()
{
	Player*        p = player.Find<Player>();
	iw::Transform* t = player.Find<iw::Transform>();

	p->fire1_timer += iw::DeltaTime();

	if (p->i_fire1 && p->fire1_timer > p->fire1_time)
	{
		p->fire1_timer = 0;

		auto [x, y, dx, dy] = GetShot(t->Position.x, t->Position.y, sand->sP.x, sand->sP.y, 1000, 10);

		//dx += iw::randf();
		//dy += iw::randf();
		
		Bus->push<SpawnProjectile_Event>(x, y, dx, dy, SpawnProjectile_Event::BULLET);
	}

	int checkHealthRad = 50;

	for (int y = -checkHealthRad; y < checkHealthRad; y++)
	for (int x = -checkHealthRad; x < checkHealthRad; x++)
	{
		int px = x + t->Position.x;
		int py = y + t->Position.y;

		iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
		if (!chunk) continue;

		iw::Cell& health = chunk->GetCell(px, py);

		if (health.Type == iw::CellType::STONE) // if health = type health
		{
			float dist = sqrt(x*x + y*y);

			if (dist < 8)
			{
				chunk->SetCell(px, py, iw::Cell());

				// repair ship, Tile needs two copies of the sprite
			}

			else {
				chunk->KeepAlive(px, py);
				health.dx = -x / dist * 15.f;
				health.dy = -y / dist * 15.f;
			}
		}
	}

	//cam_x = iw::lerp(cam_x, t.Position.x, iw::DeltaTime() * 5);
	//cam_y = iw::lerp(cam_y, t.Position.y, iw::DeltaTime() * 5);

	//sand->SetCamera(cam_x / 10, cam_y / 10, 10, 10);
}
