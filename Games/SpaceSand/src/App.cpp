#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Systems/Player.h"
#include "Systems/EnemyBase.h"
#include "Systems/Flocking.h"
#include "Systems/Projectile.h"

struct seed {
	int x = 0, y = 0;
	iw::Color c;

	seed(int x, int y, const iw::Color& c) : x(x), y(y), c(c) {}

	float distance(
		int px, int py) const
	{
		int dx = x - px,
			dy = y - py;

		return sqrt(dx*dx + dy*dy);
	}
};

seed& findClosestSeed(
	std::vector<seed>& seeds,
	int px, int py)
{
	seed* minSeed = nullptr;
	float minDist = FLT_MAX;

	for (seed& s : seeds)
	{
		float dist = s.distance(px, py);
		if (dist < minDist)
		{
			minSeed = &s;
			minDist = dist;
		}
	}

	return *minSeed;
}

std::vector<seed> genRandomSeeds(
	int count,
	int w, int h,
	int padw, int padh)
{
	std::vector<seed> seeds;

	int w2 = w / 2;
	int h2 = h / 2;

	for (int i = 0; i < count; i++)
	{
		seeds.emplace_back(
			iw::randf() * w2 + w2 + padw,
			iw::randf() * h2 + h2 + padh,
			iw::Color(iw::randf(), iw::randf(), iw::randf())
		);
	}

	for (int x = 0; x < w; x++) findClosestSeed(seeds, x,     padh).c = 0;
	for (int x = 0; x < w; x++) findClosestSeed(seeds, x, h - padh).c = 0;
	for (int y = 0; y < h; y++) findClosestSeed(seeds,     padw, y).c = 0;
	for (int y = 0; y < h; y++) findClosestSeed(seeds, w - padw, y).c = 0;

	return seeds;
}

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	
	iw::PhysicsSystem* player;
	float cam_x, cam_y;

	GameLayer(
		iw::SandLayer* sand
	) 
		: iw::Layer("Space game")
		, sand(sand)
	{}

	int Initialize() override
	{
		for (int i = 0; i < 0; i++)
		{
			int width  = iw::randi(128) + 64, width_pad  = 10, 
				height = iw::randi(128) + 64, height_pad = 10;

			std::vector<seed> seeds = genRandomSeeds(15, width, height, width_pad, height_pad);

			iw::ref<iw::Texture> tex = REF<iw::Texture>(width, height);
			unsigned* colors = (unsigned*)tex->CreateColors();

			for (int x = 0; x < width;  x++)
			for (int y = 0; y < height; y++)
			{
				colors[x + y * width] = findClosestSeed(seeds, x, y).c.to32();
			}

			iw::Entity e = sand->MakeTile(tex, true);
			e.Find<iw::Rigidbody>()->Transform.Position.x = iw::randf() * 150;
			e.Find<iw::Rigidbody>()->Transform.Position.y = iw::randf() * 150;
			//e.Find<iw::Rigidbody>()->AngularVelocity.z = iw::FixedTime() * 20 * iw::randf();
		}

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		ProjectileSystem* guns = PushSystem<ProjectileSystem>(sand);
		                         PushSystem<PlayerSystem>(sand, guns);
		                         //PushSystem<EnemyCommandSystem>(sand);
		                        // PushSystem<FlockingSystem>();

		Renderer->Device->SetClearColor(0, 0, 0, 0);

		//MakeTestBes();

		return Layer::Initialize();
	}

	void MakeTestBes() {
		auto fillBox = [&](int x, int y, int x1, int y1) {
			for (int xi = x; xi < x1; xi++)
			for (int yi = y; yi < y1; yi++)
			{
				sand->m_world->SetCell(xi, yi, iw::Cell::GetDefault(iw::CellType::ROCK));
			}
		};

		fillBox(-500,  100,  500,  500);
		fillBox(-500, -500,  500, -100);
		fillBox(-300, -500, -200,  500);
		fillBox( 200, -500,  300,  500);
	}

	void PostUpdate() override
	{
		if (iw::Keyboard::KeyDown(iw::H))
		{
			MakeTestBes();
		}

		Renderer->BeginScene();
		Renderer->DrawMesh(iw::Transform(), sand->GetSandMesh());
		Renderer->EndScene();
	}
};

App::App() : iw::Application() 
{
	int cellSize  = 2;
	int cellMeter = 10;

	iw::SandLayer* sand = PushLayer<iw::SandLayer>(cellSize, cellMeter, true);
						  PushLayer<GameLayer>(sand);
}

int App::Initialize(
	iw::InitOptions& options)
{
	iw::ref<iw::Context> context = Input->CreateContext("Game");
		
	context->MapButton(iw::SPACE, "+jump");
	context->MapButton(iw::SHIFT, "-jump");
	context->MapButton(iw::D    , "+right");
	context->MapButton(iw::A    , "-right");
	context->MapButton(iw::W    , "+forward");
	context->MapButton(iw::S    , "-forward");
	context->MapButton(iw::C    , "use");
	context->MapButton(iw::T    , "toolbox");
	context->MapButton(iw::I    , "imgui");
	context->MapButton(iw::R    , "reload");

	context->AddDevice(Input->CreateDevice<iw::Mouse>());
	context->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
	context->AddDevice(Input->CreateDevice<iw::RawMouse>());

	return Application::Initialize(options);
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		1920/**4/3*/,
		1080/**4/3*/,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
