#pragma once

#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Components/Projectile.h"
#include "Components/Asteroid.h"

#include "Events.h"
#include "ECS.h"

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
		entity MakeProjectile(
		const ShotInfo& shot)
	{
		entity entity = MakeProjectile_raw<iw::Transform, iw::Rigidbody, _cs...>(shot)
			.set<iw::Transform>(glm::vec3(shot.x, shot.y, 0));

		iw::Rigidbody& rigidbody = entity.get<iw::Rigidbody>();
		rigidbody.SetTransform(&entity.get<iw::Transform>());
		rigidbody.Velocity = glm::vec3(shot.dx, shot.dy, 0);
		rigidbody.IsKinematic = false;

		AddEntityToPhysics(entity, Physics);

		return entity;
	}

	template<
		typename... _cs>
	entity MakeProjectile_raw(
		const ShotInfo& shot)
	{
		entity e = entities().create<Projectile, _cs...>()
			.set<Projectile>(shot);
		return e;
	}

	entity MakeBullet(const ShotInfo& shot, int depth);
	entity MakeLaser (const ShotInfo& shot, int depth);
	entity MakeWattz (const ShotInfo& shot, int depth);
	entity MakeBoltz (const ShotInfo& shot, int depth);

	//entity MakeBeam  (const ShotInfo& shot, int depth);
	//void MakeExplosion(int x, int y, int r);
};
