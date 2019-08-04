#pragma once

enum EnemyType {
	SPIN
};

struct Enemy {
	EnemyType Type;
	float     Speed;
	float     FireTimeTotal;
	float     TimeToShoot;
	float     FireCooldown;
	float     FireTime;
	bool      CanShoot;
};
