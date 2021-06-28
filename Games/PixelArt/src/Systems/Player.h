#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/Components/Timer.h"
#include "../Events.h"
#include "../Player.h"

#include "SandColliders.h"

class PlayerSystem : public iw::SystemBase
{
public:
	iw::Entity PlayerEntity;
private:
	iw::Mesh m_playerMesh;
	SandColliderSystem* m_coliderSystem;

public:
	PlayerSystem(
		const iw::Mesh& playerMesh,
		SandColliderSystem* colliders
	)
		: iw::SystemBase("Player")
		, m_playerMesh(playerMesh)
		, m_coliderSystem(colliders)
	{}

	int Initialize() override;
	void Update() override;
};
