#include "iw/engine/EntryPoint.h"

#include "plugins/iw/Sand/SandWorker.h"
#include "Systems/SandWorldUpdateSystem.h"

// what does this need to do
// basic sand simulation
// allow users to add differnt types of cells if they discover a new one

using namespace iw::plugins::Sand;

class SandLayer : public iw::Layer {
private:
	iw::SandWorld m_world;

	iw::Mesh             m_sandScreenMesh;
	iw::ref<iw::Texture> m_sandScreenTexture;

	SandWorldUpdateSystem* m_sandUpdate = nullptr;
	iw::Entity player;

public:
	SandLayer()
		: iw::Layer("Sandbox")
		, m_world(200, 200, 2)
	{}

	int Initialize() override {
		Cell _EMPTY = {
			CellType::EMPTY,
			CellProperties::NONE,
			iw::Color::From255(0, 0, 0, 0)
		};

		Cell _SAND = {
			CellType::SAND,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
			iw::Color::From255(235, 200, 175)
		};

		Cell _WATER = {
			CellType::WATER,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
			iw::Color::From255(175, 200, 235)
		};

		Cell _ROCK = {
			CellType::ROCK,
			CellProperties::NONE,
			iw::Color::From255(200, 200, 200)
		};

		Cell::SetDefault(CellType::EMPTY, _EMPTY);
		Cell::SetDefault(CellType::SAND,  _SAND);
		Cell::SetDefault(CellType::WATER, _WATER);
		Cell::SetDefault(CellType::ROCK,  _ROCK);

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

		m_sandUpdate = PushSystem<SandWorldUpdateSystem>(m_world, m_sandScreenTexture);

		return 0;
	}

	void PreUpdate() {
		Tile* tile = player.Find<Tile>();

		int width  = m_sandScreenTexture->Width();
		int height = m_sandScreenTexture->Height();

		int fx = tile->X - width  / 2;
		int fy = tile->Y - height / 2;
		int fx2 = fx + width;
		int fy2 = fy + height;

		m_sandUpdate->SetCamera(fx, fy, fx2, fy2);

		if (iw::Keyboard::KeyDown(iw::LEFT))  tile->X -= iw::DeltaTime()*150;
		if (iw::Keyboard::KeyDown(iw::RIGHT)) tile->X += iw::DeltaTime()*150;
		if (iw::Keyboard::KeyDown(iw::UP))    tile->Y += iw::DeltaTime()*150;
		if (iw::Keyboard::KeyDown(iw::DOWN))  tile->Y -= iw::DeltaTime()*150;

		DrawWithMouse(fx, fy, width, height);
	}

	void PostUpdate() {
		Renderer->BeginScene(MainScene);
			Renderer->DrawMesh(iw::Transform(), m_sandScreenMesh);
		Renderer->EndScene();
	}

	void DrawWithMouse(int fx, int fy, int width, int height) {
		iw::vector2 pos = iw::Mouse::ScreenPos() / m_world.m_scale + iw::vector2(fx, -fy);
		pos.y = height - pos.y;

		if (iw::Mouse::ButtonDown(iw::LMOUSE)) {
			CellType placeMe = CellType::EMPTY;

				 if (iw::Keyboard::KeyDown(iw::S)) placeMe = placeMe = CellType::SAND;
			else if (iw::Keyboard::KeyDown(iw::W)) placeMe = placeMe = CellType::WATER;
			else if (iw::Keyboard::KeyDown(iw::R)) placeMe = placeMe = CellType::ROCK;

			for (int x = pos.x; x < pos.x + 20; x++)
			for (int y = pos.y; y < pos.y + 20; y++) {
				if (!m_world.InBounds(x, y))
					continue;

				m_world.SetCell(x, y, Cell::GetDefault(placeMe));
			}
		}

		//// Cursor, should be in ECS as another mesh

		//unsigned* pixels = (unsigned*)m_sandScreenTexture->Colors();

		//size_t mouseIndex = size_t(pos.x - fx) + size_t(pos.y - fy) * width;
		//if (mouseIndex < width * height) {
		//	pixels[mouseIndex] = iw::Color(1, 0, 0);
		//}
	}

	void InitSandScreenTexture() {
		iw::ref<iw::Shader> shader = Asset->Load<iw::Shader>("shaders/texture.shader");

		m_sandScreenTexture = REF<iw::Texture>(
			Renderer->Width()  / m_world.m_scale,
			Renderer->Height() / m_world.m_scale,
			iw::TEX_2D,
			iw::RGBA
		);
		m_sandScreenTexture->SetFilter(iw::NEAREST);
		m_sandScreenTexture->CreateColors();
		m_sandScreenTexture->Clear();

		iw::ref<iw::Material> material = REF<iw::Material>(shader);
		material->SetTexture("texture", m_sandScreenTexture);

		m_sandScreenMesh = Renderer->ScreenQuad().MakeInstance();
		m_sandScreenMesh.SetMaterial(material);
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
	return new App();
}
