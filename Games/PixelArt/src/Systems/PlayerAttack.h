#pragma once

#include "iw/engine/Layer.h"
#include "../Events.h"
#include "../Player.h"

class PlayerAttackSystem : public iw::SystemBase
{
private:
	std::unordered_map<AttackType, PlayerComboAttack> m_attacks;
public:
	PlayerAttackSystem() : iw::SystemBase("Player Attack") {}

	int Initialize();
	void Update();
	bool On(iw::ActionEvent& e);
	bool IsCombo(PlayerAttackState* player, AttackType attack, int neededCount);
};
