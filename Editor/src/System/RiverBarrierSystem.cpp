#include "Systems/RiverBarrierSystem.h"
#include "Events/ActionEvents.h"

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/PlaneCollider.h"

#include <vector>

RiverBarrierSystem::RiverBarrierSystem()
	: iw::SystemBase("River barrier")
{}

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
				&& event.LevelName.find("01")    != std::string::npos)
			{
				break;
			}

			float side = event.PlayerPosition.z > 0 ? 1 : -1;

			iw::Entity halfway = Space->CreateEntity<iw::Transform, iw::CollisionObject, iw::PlaneCollider>();

			iw::Transform*       t = halfway.Set<iw::Transform>(0);
			iw::CollisionObject* c = halfway.Set<iw::CollisionObject>();
			iw::PlaneCollider*   p = halfway.Set<iw::PlaneCollider>(side * iw::vector3::unit_z, -4);

			t->SetParent(event.Level);

			c->SetCol(p);
			c->SetTrans(t);

			Physics->AddCollisionObject(c);

			break;
		}
	}

	return false;
}
