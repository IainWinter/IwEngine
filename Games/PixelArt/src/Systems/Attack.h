#pragma once

#include "iw/engine/Layer.h"
#include "../Events.h"

// Event flow for these systems


// Enemy System - Player System
//         event_Attack
//Enemey Attck System - Player Attack System
// Attack System

class AttackSystem : public iw::SystemBase
{
public:
	bool On(iw::ActionEvent& e);
};
