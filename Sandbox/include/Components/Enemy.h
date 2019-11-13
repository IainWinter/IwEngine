#pragma once

enum EnemyType {
	SPIN
};

struct Enemy {
	EnemyType Type;
	float Speed;
	float FireTime;
	float CooldownTime;
	float Timer;
	float Rotation;
	bool  HasShot;
};
