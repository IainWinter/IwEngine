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
#include "Systems/TileSplit_System.h"
#include "Systems/Upgrade_System.h"
#include "Systems/Recording_System.h"

#include "Systems/DeathTransition_System.h"

#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"

#include "Helpers.h"

struct Game_Layer : iw::Layer
{
	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laserCharge;

	ProjectileSystem* proj_s;
	ItemSystem*       item_s;

	Game_Layer(
		iw::SandLayer* sand,
		iw::SandLayer* sand_ui_laserCharge
	) 
		: iw::Layer           ("Game")
		, sand                (sand)
		, sand_ui_laserCharge (sand_ui_laserCharge)
		, proj_s              (nullptr)
		, item_s              (nullptr)
	{}

	int Initialize() override;
	void Destroy() override;

	bool On(iw::ActionEvent& e) override;
};
