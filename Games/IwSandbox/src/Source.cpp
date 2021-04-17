#include "iw/engine/EntryPoint.h"

#include "plugins/iw/Sand/SandWorker.h"
#include "Systems/SandWorldUpdateSystem.h"
#include "iw/math/noise.h"

#include "Fluid.h"


// what does this need to do
// basic sand simulation
// allow users to add differnt types of cells if they discover a new one

using namespace iw::plugins::Sand;

class SandLayer : public iw::Layer {
private:
	SandWorld* m_world;

	iw::Mesh             m_sandScreenMesh;
	iw::ref<iw::Texture> m_sandScreenTexture;

	iw::Mesh             m_airScreenMesh;
	iw::ref<iw::Texture> m_airScreenTexture;

	SandWorldUpdateSystem* m_sandUpdate = nullptr;
	iw::Entity player;

	FluidCube* cube;
	int size = 150;

public:
	SandLayer()
		: iw::Layer("Sandbox")
	{}

	int Initialize() override {
		m_world = new SandWorld(Renderer->Width(), Renderer->Height(), 6, 4, 4);
		m_world->AddField<HeatField>();

		/*struct WindField {
			float dx, dy;
		};*/

		//m_world->AddField<WindField>();

		// Cell data
		// Field data
		//
		//

		LOG_INFO << "World chunk size: " << m_world->m_chunkWidth << ", " << m_world->m_chunkHeight;

		Cell _EMPTY = {
			CellType::EMPTY,
			CellProperties::NONE,
			CellStyle::NONE,
			iw::Color::From255(0, 0, 0, 0)
		};

		Cell _SAND = {
			CellType::SAND,
			CellProperties::MOVE_FORCE | CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
			CellStyle::RANDOM_STATIC,
			iw::Color::From255(235, 200, 175),
			iw::Color::From255(25, 25, 25, 0)
		};

		Cell _WATER = {
			CellType::WATER,
			CellProperties::MOVE_FORCE | CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
			CellStyle::SHIMMER,
			iw::Color::From255(175, 200, 235),
			iw::Color::From255(25, 25, 25, 0)
		};

		Cell _ROCK = {
			CellType::ROCK,
			CellProperties::NONE,
			CellStyle::NONE,
			iw::Color::From255(200, 200, 200)
		};

		Cell _STONE = {
			CellType::STONE,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_FORCE,
			CellStyle::RANDOM_STATIC,
			iw::Color::From255(200, 200, 200),
			iw::Color::From255(10, 10, 10, 0),
		};

		Cell _WOOD = {
			CellType::WOOD,
			CellProperties::NONE,
			CellStyle::RANDOM_STATIC,
			iw::Color::From255(171, 121, 56),
			iw::Color::From255(15, 10, 10, 0)
		};

		Cell _FIRE = {
			CellType::FIRE,
			CellProperties::MOVE_FORCE | CellProperties::BURN | CellProperties::MOVE_RANDOM,
			CellStyle::NONE,
			iw::Color::From255(255, 98, 0) // figure out how to blend colors properly, this makes it magenta because red gets overflown??? but still happens with 200
		};

		Cell _SMOKE = {
			CellType::SMOKE,
			CellProperties::MOVE_FORCE | CellProperties::MOVE_RANDOM,
			iw::CellStyle::RANDOM_STATIC,
			iw::Color::From255(100, 100, 100),
			iw::Color::From255(25, 25, 25, 0)
		};

		Cell::SetDefault(CellType::EMPTY, _EMPTY);
		Cell::SetDefault(CellType::SAND,  _SAND);
		Cell::SetDefault(CellType::WATER, _WATER);
		Cell::SetDefault(CellType::ROCK, _ROCK);
		Cell::SetDefault(CellType::STONE, _STONE);
		Cell::SetDefault(CellType::WOOD, _WOOD);
		Cell::SetDefault(CellType::FIRE, _FIRE);
		Cell::SetDefault(CellType::SMOKE,  _SMOKE);

		player = Space->CreateEntity<Tile>();
		player.Set<Tile>(Tile {{
				              {2,0},
				       {1,1}, {2,1}, 
				{0,2}, {1,2}, {2,2}, {3,2},
				{0,3}, {1,3}, {2,3}, {3,3},
				{0,4},               {3,4},
				{0,5},               {3,5},
				{0,6},               {3,6},
			},
			200, 200
		});

		InitSandScreenTexture();

		cube = FluidCubeCreate(size, 0, 0, 0.16f);

		m_sandUpdate = PushSystem<SandWorldUpdateSystem>(*m_world, m_sandScreenTexture);


		//iw::ref<iw::Shader> compute = Asset->Load<iw::Shader>("shaders/");


		return 0;
	}

	void PreUpdate() {
		Tile* tile = player.Find<Tile>();

		int width  = m_sandScreenTexture->Width();
		int height = m_sandScreenTexture->Height();

		int fx = 0;//tile->X - width / 2;
		int fy = 0;//tile->Y - height / 2;
		int fx2 = fx + width;
		int fy2 = fy + height;

		m_sandUpdate->SetCamera(fx, fy, fx2, fy2);

		if (iw::Keyboard::KeyDown(iw::LEFT))  tile->X -= iw::DeltaTime()*150;
		if (iw::Keyboard::KeyDown(iw::RIGHT)) tile->X += iw::DeltaTime()*150;
		if (iw::Keyboard::KeyDown(iw::UP))    tile->Y += iw::DeltaTime()*150;
		if (iw::Keyboard::KeyDown(iw::DOWN))  tile->Y -= iw::DeltaTime()*150;

		DrawWithMouse(fx, fy, width, height);
	}

	iw::vector2 lastP;

	void PostUpdate() {
		iw::vector2 p = iw::Mouse::ClientPos() / iw::vector2(Renderer->Width(), Renderer->Height()) * size;
		p.y = size-1 - p.y;

		p = iw::clamp<iw::vector2>(p, 1, cube->size-1);

		//FluidCubeAddDensity(cube, 50, 50, 1);
		//FluidCubeAddVelocity(cube, 50, 50, sin(iw::TotalTime()) / 10, .1);

		if(iw::Mouse::ButtonDown(iw::RMOUSE)) {

			if (iw::Keyboard::KeyDown(iw::H)) {
				for (int x = p.x-3; x < p.x + 3; x++)
				for (int y = p.y-3; y < p.y + 3; y++) {
					iw::vector2 v = p - iw::vector2(x, y);
					v.normalize();
					v *= -1;

					FluidCubeAddDensity (cube, x, y, 100);
					FluidCubeAddVelocity(cube, x, y, v.x, v.y);
				}
			}

			else {
				for (int x = p.x; x < p.x + 5; x++)
				for (int y = p.y; y < p.y + 5; y++) {
					FluidCubeAddDensity (cube, x, y, 1);
					FluidCubeAddVelocity(cube, x, y,
						iw::clamp<float>((p.x - lastP.x)/10, -1, 1),
						iw::clamp<float>((p.y - lastP.y)/10, -1, 1));
				}
			}

			lastP = p;
		}

		float visc = 10;
		float diff = .1;
		float dt = iw::DeltaTime();

		FluidCubeStep(cube);

		for (int i = 0; i < cube->size * cube->size; i++) {
			cube->density[i] = iw::clamp<float>(cube->density[i] * .99f, 0, 1);
		}

		for (SandChunk* chunk : m_world->m_chunks) {
			for (int x = 0; x < chunk->m_width;  x++) {
			for (int y = 0; y < chunk->m_height; y++) {

				int index = x + y * chunk->m_width;

				double scaleX = (double)m_airScreenTexture->Width()  / m_sandScreenTexture->Width();
				double scaleY = (double)m_airScreenTexture->Height() / m_sandScreenTexture->Height();

				int xx = (chunk->m_x + x) * scaleX;
				int yy = (chunk->m_y + y) * scaleY;

				int i = xx + yy * cube->size;

				Cell& cell = chunk->GetCell(index);
				float& temp = chunk->GetCell<HeatField>(index, 1).Tempeture;

				if (cell.Type == CellType::FIRE) {
					FluidCubeAddDensity (cube, xx, yy, iw::DeltaTime() / 2);
					FluidCubeAddVelocity(cube, xx, yy, 
						(sin(iw::TotalTime() * 50) + iw::randf()) * iw::DeltaTime() / 100, 
						iw::DeltaTime() / 50
					);
				}

				if (cell.Type == CellType::SAND && cube->density[i] > .8 && temp > 800 && iw::randf() > .9f) {
					chunk->SetCell(index, Cell::GetDefault(CellType::STONE));
				}

				if (cell.Props & CellProperties::MOVE_FORCE) {
					//if (!(cell.Props & CellProperties::MOVE_RANDOM)) {
					//	cell.dy -= iw::DeltaTime() * 9.8f;
					//}
				}

				else {
					continue;
				}

				if (cube->density[i] > 0.001f) {
					int index = x + y * chunk->m_width;

					Cell& cell = chunk->GetCell(index);

					if (cell.Type != iw::CellType::EMPTY) {
						cell.dx += cube->Vx[i] * sqrt(cube->density[i])*4;
						cell.dy += cube->Vy[i] * sqrt(cube->density[i])*4;

						chunk->KeepAlive(index);
					}
				}
			}
			}
		}

		// Rendering

		unsigned* colors = (unsigned*)m_airScreenTexture->Colors();

		for (int i = 0; i < cube->size*cube->size; i++) {
			iw::Color c(iw::clamp<float>(cube->density[i], 0, 1));
			colors[i] = c.to32();
		}

		colors[int(p.x) + int(p.y) * cube->size] = iw::Color(1).to32();

		m_airScreenTexture ->Update(Renderer->Device);
		m_sandScreenTexture->Update(Renderer->Device);

		iw::vector3 aspect = iw::vector3(float(Renderer->Height()) / Renderer->Width(), 1, 1);

		Renderer->BeginScene(MainScene);
			Renderer->DrawMesh(iw::Transform(0, aspect), m_sandScreenMesh);
			Renderer->DrawMesh(iw::Transform(0, aspect), m_airScreenMesh);
		Renderer->EndScene();

		//while (!iw::Keyboard::KeyDown(iw::N)) {

		//}
	}

	void DrawWithMouse(int fx, int fy, int width, int height) {
		iw::vector2 pos = iw::Mouse::ClientPos() / m_world->m_scale /*/ m_world->m_scale*/ + iw::vector2(fx, -fy);
		pos.y = height - pos.y;

		if (iw::Mouse::ButtonDown(iw::LMOUSE)) {
			CellType placeMe = CellType::EMPTY;

				 if (iw::Keyboard::KeyDown(iw::S)) placeMe = CellType::SAND;
			else if (iw::Keyboard::KeyDown(iw::W)) placeMe = CellType::WATER;
			else if (iw::Keyboard::KeyDown(iw::R)) placeMe = CellType::ROCK;
			else if (iw::Keyboard::KeyDown(iw::T)) placeMe = CellType::STONE;
			else if (iw::Keyboard::KeyDown(iw::F)) placeMe = CellType::FIRE;
			else if (iw::Keyboard::KeyDown(iw::M)) placeMe = CellType::SMOKE;
			else if (iw::Keyboard::KeyDown(iw::O)) placeMe = CellType::WOOD;

			for (int x = pos.x; x < pos.x + 10; x++)
			for (int y = pos.y; y < pos.y + 10; y++) {
				if (!m_world->InBounds(x, y))
					continue;

				Cell cell = Cell::GetDefault(placeMe);
				cell.StyleOffset = iw::randf();

				m_world->SetCell(x, y, cell);
			}
		}
	}

	void InitSandScreenTexture() {
		m_sandScreenMesh = CreateScreenTextureMesh(Renderer->Width() / m_world->m_scale, Renderer->Height() / m_world->m_scale, true);
		m_sandScreenTexture = m_sandScreenMesh.Material()->GetTexture("texture");

		m_airScreenMesh = CreateScreenTextureMesh(size, size, false);
		m_airScreenTexture = m_airScreenMesh.Material()->GetTexture("texture");
	}

	iw::Mesh CreateScreenTextureMesh(int width, int height, bool useAlpha) {
		iw::Mesh mesh = Renderer->ScreenQuad().MakeInstance();
		
		iw::ref<iw::Texture> texture = REF<iw::Texture>(width, height, iw::TEX_2D, iw::RGBA);
		iw::ref<iw::Shader> shader = Asset->Load<iw::Shader>("shaders/texture.shader");
		iw::ref<iw::Material> material = REF<iw::Material>(shader);

		texture->SetFilter(iw::NEAREST);
		texture->CreateColors();
		texture->Clear();

		material->SetTexture("texture", texture);
		material->Set("useAlpha", (int)useAlpha);
		mesh.SetMaterial(material);

		return mesh;
	}
};

class App : public iw::Application {
public:
	int Initialize(iw::InitOptions& options) override {
		PushLayer<SandLayer>();
		return iw::Application::Initialize(options);
	}
};

iw::Application* CreateApplication(iw::InitOptions& options) {
	options.WindowOptions.Width  = 1280;
	options.WindowOptions.Height = 1280;
	return new App();
}
