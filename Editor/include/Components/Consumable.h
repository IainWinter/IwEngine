#pragma once

enum ConsumableType {
	SLOWMO,     // Slow time for a second
	CHARGE_KILL // Large bullet kill rad
};

struct Consumable {
	unsigned Index;
	ConsumableType Type;

	float Time;
	float Timer;
};

struct Slowmo {};
struct ChargeKill {};
