#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/AABB.h"

#include "IW/physics/Collision/SphereCollider.h";
#include "IW/physics/Dynamics/Rigidbody.h";

EnemySystem::EnemySystem(
	iw::ref<IW::Model> bulletModel)
	: IW::System<IW::Transform, Enemy>("Enemy")
	, BulletModel(bulletModel)
{}

void EnemySystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::PI2);
			iw::quaternion rot = iw::quaternion::from_euler_angles(0, enemy->Rotation, 0);

			iw::vector3 v = iw::vector3::unit_x * rot;
			v.normalize();
			v *= 5;

			IW::Entity ent = Space->CreateEntity<IW::Transform, IW::Model, IW::SphereCollider, IW::Rigidbody, Bullet>();
			Space->SetComponentData<IW::Model>(ent, *BulletModel);
			Space->SetComponentData<Bullet>   (ent, LINE, 5.0f);

			IW::Transform* t      = Space->SetComponentData<IW::Transform>     (ent, transform->Position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
			IW::SphereCollider* s = Space->SetComponentData<IW::SphereCollider>(ent, iw::vector3::zero, .25f);
			IW::Rigidbody* r      = Space->SetComponentData<IW::Rigidbody>     (ent);

			r->SetMass(1);
			r->SetCol(s);
			r->SetTrans(t);
			r->SetVelocity(v);
			r->SetDynamicFriction(0);
			r->SetSimGravity(false);

			Physics->AddRigidbody(r);
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= IW::Time::DeltaTime();
		}
	}
}
