#include "Attack.h"
#include "iw/engine/ComponentHelper.h"
#include "iw/engine/Components/Timer.h"

struct func_Attack {
	iw::ref<iw::Space> Space;
	iw::EntityHandle Source;

	glm::vec2 Velocity;
	std::function<bool(iw::EntityHandle)> func_deleteIf;

	func_Attack(
		iw::ref<iw::Space> space,
		iw::EntityHandle source,
		glm::vec2 velocity,
		std::function<bool(iw::EntityHandle)> func_deleteIf = [](iw::EntityHandle) { return false; }
	)
		: Space(space)
		, Source(source)
		, Velocity(velocity)
		, func_deleteIf(func_deleteIf)
	{}

	void operator()(
		iw::Manifold& manifold,
		iw::scalar dt)
	{
		iw::Entity other, hitbox;

		if (iw::GetEntitiesFromManifold<Attack>(Space, manifold, hitbox, other)) {
			return;
		}

		if (hitbox.Find<Attack>()->Source == other.Handle) { // Exit if colliding with source
			return;
		}

		iw::Rigidbody*       e = other .Find<iw::Rigidbody>();
		iw::CollisionObject* b = hitbox.Find<iw::CollisionObject>();

		e->Velocity.x = Velocity.x;
		e->Velocity.y = Velocity.y;

		if (   func_deleteIf
			&& func_deleteIf(Source)) 
		{
			Space->QueueEntity(hitbox.Handle, iw::func_Destroy);
		}
	}
};

void AttackSystem::Update()
{
	Space->Query<iw::Timer, Attack>().Each([&](
		iw::EntityHandle e,
		iw::Timer* timer,
		Attack*)
	{
		timer->Tick();
		if (timer->Can("remove")) Space->QueueEntity(e, iw::func_Destroy);
	});
}

bool AttackSystem::On(
	iw::ActionEvent& e)
{
	if (e.Type == iw::val(myActionEvents::ATTACK))
	{
		event_Attack& event = e.as<event_Attack>();
		MakeAttack(event.Props, event.Facing, event.Entity);
	}

	return false;
}

void AttackSystem::MakeAttack(
	AttackProps& props,
	float facing,
	iw::EntityHandle source)
{
	iw::Entity attack = Space->CreateEntity<
		iw::Transform, 
		iw::CollisionObject, 
		iw::Hull2,
		iw::Timer,
		iw::Mesh, // debug
		Attack>();

	attack.Set<iw::Mesh>(m_square);

	attack.Set<Attack>(source);

	iw::Transform*       transform = attack.Set<iw::Transform>(props.Transform);
	iw::CollisionObject* object    = attack.Set<iw::CollisionObject>();
	iw::Hull2*           collider  = attack.Set<iw::Hull2>(props.Hitbox);
	iw::Timer*           timer     = attack.Set<iw::Timer>();
	                                 
	Physics->AddCollisionObject(object);

	transform->SetParent(Space->FindComponent<iw::Transform>(source));
	
	transform->Position.x *= facing;
	props.Knockback.x *= facing;

	object->SetTransform(transform);
	object->Collider = collider;
	object->IsTrigger = true;
	object->OnCollision = func_Attack(
		Space,
		source,
		props.Knockback, 
		props.func_deleteIf
	);

	timer->SetTime("remove", props.Time);
}
