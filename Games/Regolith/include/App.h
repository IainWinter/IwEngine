#pragma once 

#include "Events.h"
//#include "iw/util/reflection/serialization/Serializer.h"
//#include "iw/reflected/reflected.h"

#include "iw/engine/EntryPoint.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Layers/Game_Layer.h"

struct MenuLayer : iw::Layer
{
	MenuLayer() : iw::Layer("Menu layer") {}

	void PostUpdate();
};

class App : public iw::Application {
private:
	StateName state;

	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laser;
	Game_Layer* game;
	MenuLayer* menu;

public:
	App();
	int Initialize(iw::InitOptions& options) override;
};
