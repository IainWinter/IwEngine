#include "Systems/RiverBarrierSystem.h"
#include "Events/ActionEvents.h"

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/PlaneCollider.h"

#include <vector>

RiverBarrierSystem::RiverBarrierSystem(
	iw::Entity& player)
	: iw::SystemBase("River barrier")
	, m_player(player)
{}

void RiverBarrierSystem::Update() {
	if (m_halfway) {
		iw::PlaneCollider* p = m_halfway.Find<iw::PlaneCollider>();

		if (p) {
			iw::Transform* t = m_player.Find<iw::Transform>();

			if (t) {
				float side = t->WorldPosition().z > 0 ? 1 : -1;
				p->Plane.P = side * iw::vector3::unit_z;
			}
		}
	}
}

bool RiverBarrierSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			for (iw::Entity b : m_barriers) { // quick & dirty should just add a component
				b.Destroy();
			}

			StartLevelEvent& event = e.as<StartLevelEvent>();
			
			if (   event.LevelName.find("river") != std::string::npos
				&& event.LevelName.find("01")    != std::string::npos
				&& event.LevelName.find("11")    != std::string::npos)
			{
				break;
			}

			float side = event.PlayerPosition.z > 0 ? 1 : -1;

			if (m_halfway) {
				Space->QueueEntity(m_halfway.Handle, iw::func_Destroy);
			}

			m_halfway = Space->CreateEntity<iw::Transform, iw::CollisionObject, iw::PlaneCollider>();

			iw::Transform*       t = m_halfway.Set<iw::Transform>(0);
			iw::CollisionObject* c = m_halfway.Set<iw::CollisionObject>();
			iw::PlaneCollider*   p = m_halfway.Set<iw::PlaneCollider>(side * iw::vector3::unit_z, -4);

			t->SetParent(event.Level);

			c->SetCol(p);
			c->SetTrans(t);

			Physics->AddCollisionObject(c);

			break;
		}
	}

	return false;
}
