#include "Attack.h"

bool AttackSystem::On(
	iw::ActionEvent& e)
{
	if (e.Type == iw::val(myActionEvents::ATTACK))
	{
		event_Attack& event = e.as<event_Attack>();

		// MakeAttack
	}

	return false;
}
