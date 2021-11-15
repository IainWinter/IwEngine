#pragma once

#include "iw/engine/Components/Timer.h"
#include "iw/common/Components/Transform.h"
#include "iw/entity/Entity.h"
#include "Components/ProjHead.h"

enum WeaponType {
	DEFAULT_CANNON,
	MINIGUN_CANNON,
	FAT_LASER_CANNON,
	LASER_CANNON,
	SPECIAL_BEAM_CANNON,
	WATTZ_CANNON,
	BOLTZ_CANNON
};

class Weapon {
private:
	float m_lastShot = 0.f;

public:
	WeaponType Type;

	float FireDelay = 0.f;
	float FireDelayMargin = 0.f;

	int Ammo = -1; // negitive is infinity

	std::string Audio;

	Weapon(WeaponType type)
		: Type(type)
	{}

	bool CanFire()
	{
		if (Ammo == 0) return false;

		bool canFire = iw::TotalTime() - m_lastShot > FireDelay;
		if (canFire) {
			m_lastShot = iw::TotalTime() + FireDelayMargin * iw::randf();
		}

		return canFire;
	}

	virtual ShotInfo GetShot(iw::Entity origin, float targetX, float targetY) = 0;
};

struct Cannon : Weapon
{
	float BreadthFromOrigin  = 0.f;
	float DistanceFromOrigin = 0.f;
	float Inaccuracy = 0.f;
	float Speed = 0.f;
	float SpeedMargin = 0.f;

	ProjectileType Projectile;

	Cannon(
		WeaponType weapon,
		ProjectileType projectile
	)
		: Weapon(weapon)
		, Projectile(projectile)
	{}

	ShotInfo GetShot(
		iw::Entity origin,
		float targetX, float targetY) override
	{
		Ammo -= 1;

		glm::vec3& pos = origin.Find<iw::Transform>()->Position;

		ShotInfo shot = GetShot_Circular(
			pos  .x, pos  .y,
			targetX, targetY,
			Speed + SpeedMargin * iw::randf(),
			DistanceFromOrigin,
			BreadthFromOrigin
		);

		shot.origin = origin;

		float speed = shot.Speed();

		shot.dx += speed * Inaccuracy * iw::randfs();
		shot.dy += speed * Inaccuracy * iw::randfs();
		shot.projectile = Projectile;

		return shot;
	}
};

// should put in cpp

inline Weapon* MakeDefault_Cannon()
{
	Cannon* cannon = new Cannon(WeaponType::DEFAULT_CANNON, ProjectileType::BULLET);
	cannon->Speed = 1250;
	cannon->DistanceFromOrigin = 15;
	cannon->BreadthFromOrigin = 2;
	cannon->FireDelay = 0.15;
	cannon->Audio = "event:/weapons/fire_cannon";

	return cannon;
}

inline Weapon* MakeWattz_Cannon()
{
	Cannon* cannon = new Cannon(WeaponType::WATTZ_CANNON, ProjectileType::WATTZ);
	cannon->Speed = 200;
	cannon->DistanceFromOrigin = 15;
	cannon->BreadthFromOrigin = 2;
	cannon->FireDelay = 0.5;
	cannon->Ammo = 10;

	return cannon;
}

inline Weapon* MakeBoltz_Cannon()
{
	Cannon* cannon = new Cannon(WeaponType::BOLTZ_CANNON, ProjectileType::BOLTZ);
	cannon->Speed = 150;
	cannon->DistanceFromOrigin = 15;
	cannon->BreadthFromOrigin = 2;
	cannon->FireDelay = 0.005;
	cannon->Ammo = 1000;

	return cannon;
}

inline Weapon* MakeMinigun_Cannon()
{
	Cannon* cannon = new Cannon(WeaponType::MINIGUN_CANNON, ProjectileType::BULLET);
	cannon->Speed = 1250 / 4.f;
	cannon->DistanceFromOrigin = 15;
	cannon->BreadthFromOrigin = 2;
	cannon->FireDelay = 0.15 / 3.f;
	cannon->Inaccuracy = .1f;
	cannon->Ammo = 200;

	return cannon;
}

inline Weapon* MakeFatLaser_Cannon()
{
	Cannon* cannon = new Cannon(WeaponType::FAT_LASER_CANNON, ProjectileType::LASER);
	cannon->Speed = 1800;
	cannon->SpeedMargin = 400;
	cannon->DistanceFromOrigin = 15;
	cannon->BreadthFromOrigin = 7;
	cannon->FireDelay = 0.015;

	return cannon;
}

// enemy guns are slower to make the game fair. not sure how to name those, i want the enemy
// to be able to use all the same guns as the player

inline Weapon* MakeLaser_Cannon_Enemy()
{
	Cannon* cannon = new Cannon(WeaponType::LASER_CANNON, ProjectileType::LASER);
	cannon->Speed = 444;
	cannon->SpeedMargin = 40;
	cannon->DistanceFromOrigin = 15;
	cannon->BreadthFromOrigin = 4;
	cannon->FireDelay = 2;
	cannon->FireDelayMargin = 1;
	cannon->Inaccuracy = .05f;

	return cannon;
}

inline Weapon* MakeSpecialBeam_Cannon()
{
	Cannon* cannon = new Cannon(WeaponType::SPECIAL_BEAM_CANNON, ProjectileType::BEAM);
	cannon->Speed = 10000;
	cannon->DistanceFromOrigin = 15;
	cannon->FireDelay = .5;
	cannon->Ammo = 1;

	return cannon;
}

