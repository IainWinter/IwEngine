#pragma once

enum class PlayerLocationState {
	GROUND,
	AIR
};

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

	PlayerLocationState Location = PlayerLocationState::GROUND;
	std::vector<std::pair<AttackType, size_t>> PreviousAttacks; // type, tick
};


struct PlayerComboAttack {
	AttackProps Normal;
	AttackProps Combo;
	int NeededComboCount = -1;
};
