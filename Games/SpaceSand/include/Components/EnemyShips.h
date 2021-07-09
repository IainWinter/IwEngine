#pragma once

#include "Components/EnemyBase.h"
#include "glm/vec2.hpp"

struct EnemySupplyShip { // attack mode = flee
	int MaxRez = 1500; // 3x return
	int CapturedRez = 0;
};

struct EnemyAttackShip { }; // Tag, attack mode = fight player

struct EnemyShip {
	EnemyBase* Command; // To request commands from
	float ChecekCommandTimer = 0;
	float ChecekCommandTime = 5.0f;

	bool RezLow = false;
	bool AtObjective = false;
	bool AttackMode = false;

	bool PlayerVisible = false;

	bool Homecoming = false;
	bool WantGoHome = false;

	glm::vec2 Objective; // Current location to move twoards (get from Command), should just use front of Objectives
	std::vector<glm::vec2> Objectives; // Random location to move to, same for each group spawned

	//iw::vector2 Velocity  = 0;
	float Speed = 200;

	float TurnRad = 0.025f;
	float FireTimer = 0;
	float FireTime = 0.5f;

	glm::vec2 FireDirection = glm::vec2(0);

	int Rez = 100; // Start with 100 + whatever the cost was to make
	int MinRez = 10;

	int RezToFireLaser = 5;
};
