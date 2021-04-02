#pragma once

#include "iw/engine/System.h"
#include "Sand/SandWorker.h"

// update & render, render could be its own system

class SandUpdateSystem : public iw::SystemBase {
private:
	iw::Scene* m_scene;

	SandWorld& m_world;
	iw::EntityHandle m_cameraEntity;
	float m_timeStep;
	bool m_debugStep;

	float m_stepTimer = 0.0;

	iw::Mesh m_sandMesh;

public:
	SandUpdateSystem(
		iw::Scene* scene,
		SandWorld& world,
		iw::EntityHandle cameraEntity,
		float timeStep = 0.0f,
		bool debugStep = false);

	int Initialize();
	void Update();
};
