#pragma once

#include "iw/engine/Layer.h"
#include "../Events.h"

// Event flow for these systems


// Enemy System - Player System
//         event_Attack
//Enemey Attck System - Player Attack System
// Attack System

class AttackSystem : public iw::SystemBase
{
private:
	iw::Mesh m_square; // temp for debug, should use draw collider system
public:
	AttackSystem(
		const iw::Mesh& square
	)
		: iw::SystemBase("Attack")
		, m_square(square)
	{}

	void Update() override;
	bool On(iw::ActionEvent& e) override;
	void MakeAttack(AttackProps& props, float facing, iw::EntityHandle source);
};
