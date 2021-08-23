#pragma once

#include "iw/entity/Entity.h"
#include "iw/common/Components/Transform.h"
#include "Components/Projectile.h"
#include <unordered_map>
#include <functional>

enum WeaponType
{
	CANNON,
	MINGUN,
	SUPER_LASER
};

struct ShotInfo {
	float x, y, dx, dy;
	ProjectileType projectile;
};

struct WeaponInfo;

using func_getShot = std::function<ShotInfo(iw::Entity, const WeaponInfo&, float, float)>;

struct WeaponInfo {
	float FireTime;
	float FireTimeMargin;

	func_getShot GetShot;

	float Inaccuracy;
	float Speed;
};

struct Armorments {
	WeaponType CurrentWeapon;
	int Ammo;                  // -1 = infinite

	iw::Timer Timer;

	std::unordered_map<WeaponType, WeaponInfo> Weapons;

	Armorments() {}

	WeaponInfo* GetWeapon(
		WeaponType weapon)
	{
		auto itr = Weapons.find(weapon);
		if (itr == Weapons.end())
		{
			LOG_WARNING << "Tried to get a weapon not in armorments!";
			return nullptr;
		}
		return &itr->second;
	}
};

inline std::tuple<float, float, float, float> GetShot_Circular(
	float x,  float y,
	float tx, float ty,
	float speed, float margin, float thickness = 0)
{
	float dx = tx - x,
	      dy = ty - y;

	float length = sqrt(dx*dx + dy*dy);

	float nx = dx / length,
           ny = dy / length;

	dx = nx * speed;
	dy = ny * speed;

	float r = iw::randfs();

	x += nx * margin - ny * thickness * r;
	y += ny * margin + nx * thickness * r;

	return std::tie(x, y, dx, dy);
}


inline func_getShot MakeGetShot_Circular(
	float distanceFromOrigin,
	float breadthFromOrigin,
	ProjectileType projectile)
{
	return [=](iw::Entity e, const WeaponInfo& me, float tx, float ty)
	{
		glm::vec3 p = e.Find<iw::Transform>()->Position;
		auto [x, y, dx, dy] = GetShot_Circular(p.x, p.y, tx, ty,
			me.Speed,
			distanceFromOrigin,
			breadthFromOrigin
		);

		dx += me.Speed * me.Inaccuracy * iw::randfs();
		dx += me.Speed * me.Inaccuracy * iw::randfs();

		return ShotInfo{ x, y, dx, dy, projectile };
	};
}

// these could load from a file, then in assets have an A_weapon_player_cannon

inline WeaponInfo MakeCannonInfo()
{
	WeaponInfo info;
	info.FireTime       = 0.15f;
	info.FireTimeMargin = 0.f;
	info.Speed          = 1250;
	info.Inaccuracy     = 0.05f;
	info.GetShot    = MakeGetShot_Circular(10, 2, ProjectileType::BULLET);

	return info;
}

inline WeaponInfo MakeLaserInfo()
{

}
