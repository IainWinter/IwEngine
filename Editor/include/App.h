#pragma once 

#include "iw/engine/EntryPoint.h"

class App : public iw::Application {
public:
	App();

	int Initialize(
		iw::InitOptions& options) override;

	//void Update() override;
	//void FixedUpdate() override;

	bool HandleCommand(
		const iw::Command& command) override;

	//void HandleCollision(
	//	Manifold& manifold,
	//	scalar dt) override;
};
