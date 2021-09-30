#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/physics/Collision/SphereCollider.h"

#include "Components/Projectile.h"

#include "Events.h"

struct ProjectileSystem
	: iw::SystemBase
{
	iw::SandLayer* sand;

	ProjectileSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Projectile")
		, sand(sand)
	{}

	void Update() override;

	bool On(iw::ActionEvent& e) override;

private:
	template<
		typename... _cs>
	iw::Entity MakeProjectile(
		const ShotInfo& shot)
	{
		iw::Entity entity = Space->CreateEntity<
			iw::Transform, 
			iw::Rigidbody,
			Projectile,
			_cs...>();

		Projectile* projectile = entity.Set<Projectile>(shot);

		iw::Transform* transform = entity.Set<iw::Transform>();
		iw::Rigidbody* rigidbody = entity.Set<iw::Rigidbody>();

		transform->Position = glm::vec3(shot.x, shot.y, 0);

		rigidbody->SetTransform(transform);
		rigidbody->Velocity = glm::vec3(shot.dx, shot.dy, 0);

		Physics->AddRigidbody(rigidbody);

		return entity;
	}

	template<
		typename... _cs>
	iw::Entity MakeProjectile_raw(
		const ShotInfo& shot)
	{
		iw::Entity entity = Space->CreateEntity<Projectile,_cs...>();
		Projectile* projectile = entity.Set<Projectile>(shot);
		return entity;
	}

	iw::Entity MakeBullet(const ShotInfo& shot, int depth);
	iw::Entity MakeLaser (const ShotInfo& shot, int depth);
	iw::Entity MakeWattz (const ShotInfo& shot, int depth);
	iw::Entity MakeBoltz (const ShotInfo& shot, int depth);

	iw::Entity MakeBeam  (const ShotInfo& shot, int depth);

	void MakeExplosion(int x, int y, int r);
};
