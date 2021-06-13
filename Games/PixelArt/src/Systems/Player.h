#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/Components/Timer.h"
#include "../Events.h"

struct Player {
	bool i_jump,
		i_up,
		i_down,
		i_left,
		i_right,
		i_light,
		i_heavy;

	bool OnGround = false;
	float Facing = 1;
};

class PlayerSystem : public iw::SystemBase
{
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
