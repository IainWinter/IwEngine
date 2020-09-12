#pragma once

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Color.h"
#include "iw/engine/Time.h"
#include "iw/entity/Entity.h"
#include "iw/events/eventbus.h"
#include <functional>

enum ConsumableType {
	SLOWMO,      // Slow time for a second
	CHARGE_KILL, // Large bullet kill
	LONG_DASH,   // Dash over gaps
};

struct Consumable;

using func_ConsumableAction = std::function<void(Consumable*, iw::Entity&, iw::eventbus*, bool)>;
using func_ConsumableEffect = std::function<iw::Transform(Consumable*, iw::Entity&)>;

struct Consumable {
	unsigned Index; // id
	ConsumableType Type;

	iw::Color Color;
	func_ConsumableAction Action;
	func_ConsumableEffect Effect;

	float Time;
	float Timer;
};

struct Slowmo {};
struct ChargeKill {};

namespace detail {
	void action_Slowmo(Consumable* slowmo, iw::Entity& target, iw::eventbus* bus, bool finish);
	void action_Charge(Consumable* slowmo, iw::Entity& target, iw::eventbus* bus, bool finish);
	void action_LgDash(Consumable* slowmo, iw::Entity& target, iw::eventbus* bus, bool finish);

	iw::Transform effect_Slowmo(Consumable* slowmo, iw::Entity& target);
	iw::Transform effect_Charge(Consumable* slowmo, iw::Entity& target);
}
