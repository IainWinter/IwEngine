#pragma once

#include "iw/engine/Layer.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

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

#include "Systems/Explosion_System.h"

#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"

#include "Helpers.h"

struct Game_Layer : iw::Layer
{
	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laserCharge;
	
	iw::Entity m_player;
	iw::Entity m_cursor;

	PlayerSystem*          player_s;
	PlayerLaserTankSystem* playerTank_s;
	WorldSystem*           world_s;
	KeepInWorldSystem*     keepInWorld_s;
	CorePixelsSystem*      corePixels_s;
	ProjectileSystem*      projectile_s;
	EnemySystem*           enemy_s;
	ItemSystem*            item_s;
	ScoreSystem*           score_s;
	FlockingSystem*        flocking_s;
	ExplosionSystem*       explosion_s;

	// temp ui

	int laserFluidToCreate = 0;
	int laserFluidToRemove = 0;
	int laserFluidCount = 0;
	glm::vec3 laserFluidVel = glm::vec3(0.f);
	float canFireTimer = 0;

	Game_Layer(
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
