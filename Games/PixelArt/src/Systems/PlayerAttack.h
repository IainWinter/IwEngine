#pragma once

#include "iw/engine/Layer.h"
#include "../Events.h"

enum class PlayerLocationState {
	GROUND,
	AIR
};

struct PlayerAttackState {
	PlayerLocationState Location = PlayerLocationState::GROUND;
	std::vector<AttackType> PreviousAttacks;

	float PreviousAttackTime  = 0.0;
	float PreviousAttackDelay = 1.0;
};

struct PlayerComboAttack {
	AttackProps Normal;
	AttackProps Combo;
	int NeededComboCount;
};

class PlayerAttackSystem : public iw::SystemBase
{
private:
	std::unordered_map<AttackType, PlayerComboAttack> m_attacks;
public:
	int Initialize();
	void Update();

	bool On(iw::ActionEvent& e);
private:

	bool IsCombo(PlayerAttackState* player, AttackType attack, int neededCount);
};
