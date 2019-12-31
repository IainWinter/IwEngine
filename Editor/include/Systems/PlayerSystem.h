#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace IW {
	class PlayerSystem
		: public System<Transform, Rigidbody, Player>
	{
	public:
		struct Components {
			Transform* Transform;
			Rigidbody* Rigidbody;
			Player*    Player;
		};
	private:
		iw::vector3 movement;
		bool dash;

	public:
		PlayerSystem();

		void Update(
			EntityComponentArray& view) override;

		bool On(
			KeyEvent& event);
	};
}
