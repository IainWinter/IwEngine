#pragma once

struct Player {
	bool i_jump,
		i_up,
		i_down,
		i_left,
		i_right,
		i_light,
		i_heavy;

	bool OnGround = true;
	float Facing = 1;
};

enum class PlayerLocationState {
	GROUND,
	AIR
};

struct PlayerAttackState {
	PlayerLocationState Location = PlayerLocationState::GROUND;
	std::vector<AttackType> PreviousAttacks;

	float PreviousAttackTime = 0.0;
	float PreviousAttackDelay = 1.0;
};

struct PlayerComboAttack {
	AttackProps Normal;
	AttackProps Combo;
	int NeededComboCount;
};
