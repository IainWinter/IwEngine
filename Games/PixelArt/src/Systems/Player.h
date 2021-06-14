#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/Components/Timer.h"
#include "../Events.h"
#include "../Player.h"

class PlayerSystem : public iw::SystemBase
{
public:
	iw::Entity PlayerEntity;
private:
	iw::Mesh m_playerMesh;

public:
	PlayerSystem(
		const iw::Mesh& playerMesh
	)
		: iw::SystemBase("Player")
		, m_playerMesh(playerMesh)
	{}

	int Initialize() override;
	void Update() override;
};
