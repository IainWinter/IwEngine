#pragma once 

#include "iw/engine/EntryPoint.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Systems/World_System.h"
#include "Systems/KeepInWorld_System.h"
#include "Systems/Item_System.h"

#include "Systems/Player_System.h"
#include "Systems/PlayerLaserTank_System.h"
#include "Systems/Enemy_System.h"
#include "Systems/CorePixels_System.h"
#include "Systems/Score_System.h"

#include "Systems/Flocking_System.h"
#include "Systems/Projectile_System.h"

#include "iw/graphics/Font.h"

#include "Events.h"

struct MenuLayer : iw::Layer
{
	MenuLayer() : iw::Layer("Menu layer") {}

	void PostUpdate();
};

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laserCharge;
	
	iw::Entity m_player;
	iw::Entity m_cursor;

	PlayerSystem*          player_s;
	PlayerLaserTankSystem* playerTank_s;
	WorldSystem*           world_s;
	ProjectileSystem*      projectile_s;
	EnemySystem*           enemy_s;
	ItemSystem*            item_s;
	KeepInWorldSystem*     keepInWorld_s;
	ScoreSystem*           score_s;
	CorePixelsSystem*      corePixels_s;
	FlockingSystem*        flocking_s;

	// temp ui
	bool showGameOver = false;

	int CachedAmmo  = -1;
	int CachedScore = -1;

	iw::Camera* cam;
	int laserFluidToCreate = 0;
	int laserFluidToRemove = 0;
	int laserFluidCount = 0;
	glm::vec3 laserFluidVel = glm::vec3(0.f);
	float canFireTimer = 0;
	float uiJitterAmount = 0;

	bool uiHideBar = 0;
	float uiBarOffset = 0;

	GameLayer(
		iw::SandLayer* sand,
		iw::SandLayer* sand_ui_laserCharge
	) 
		: iw::Layer("Space game")
		, sand(sand)
		, sand_ui_laserCharge(sand_ui_laserCharge)
	{}

	int Initialize() override;

	void Update() override;
	void PostUpdate() override;

	bool On(iw::ActionEvent& e) override;
};

class App : public iw::Application {
private:
	StateName state;

	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laser;
	GameLayer* game;
	MenuLayer* menu;

public:
	App();
	int Initialize(iw::InitOptions& options) override;
};

 // Helpers

std::string itos(int numb)
{
	std::stringstream buf;
	if (numb >= 0) buf << numb;
	else           buf << " "; // need space for UpdateMesh

	return buf.str();
}



// UI testing


//struct UIConstraint 
//{
//	virtual float Get(int parent) const = 0;
//	virtual ~UIConstraint() = default;
//};

//struct Fixed : UIConstraint
//{
//	float value;

//	Fixed(float value_) {
//		value = value_;
//	}

//	float Get(int parent) const override
//	{
//		return value;
//	}
//};

//struct Ratio : UIConstraint
//{
//	float value;

//	Ratio(float value) : value(value) {}

//	float Get(int parent) const override
//	{
//		return parent * value;
//	}
//};

struct UI
{
	float x, y, z, width, height;
	iw::Mesh mesh;

	UI(const iw::Mesh& mesh_)
	{
		mesh = mesh_;
		x = 0;
		y = 0;
		z = 0;
		width = 0;
		height = 0;
	}

	iw::Transform GetTransform(int screenWidth, int screenHeight, int screenDepth)
	{
		iw::Transform transform;
		transform.Position.x = floor(x)      / screenWidth;
		transform.Position.y = floor(y)      / screenHeight;
		transform.Position.z = z / screenDepth;
		transform.Scale.x    = floor(width)  / screenWidth;
		transform.Scale.y    = floor(height) / screenHeight;

		return transform;
	}
};

struct UIScreen
{
	std::vector<UI*> elements;
	int width, height, depth;

	UIScreen(int width_, int height_, int depth_)
	{
		width  = width_;
		height = height_;
		depth = depth_;
	}

	~UIScreen()
	{
		for (UI* ui : elements)
		{
			delete ui;
		}
	}

	UI* CreateElement(const iw::Mesh& mesh)
	{
		return elements.emplace_back(new UI(mesh));
	}

	void AddElement(UI* element)
	{
		elements.push_back(element);
	}

	void Draw(iw::Camera* camera, iw::ref<iw::QueuedRenderer>& renderer)
	{
		renderer->BeginScene(camera);

		for (UI* ui : elements)
		{
			renderer->DrawMesh(ui->GetTransform(width, height, depth), ui->mesh);
		}

		renderer->EndScene();
	}
};
