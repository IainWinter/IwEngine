#pragma once

enum ConsumableType {
	SLOWMO, // Slow time for a second
	CHARGE_KILL
};

struct Consumable {
	ConsumableType Type;
	bool IsActive;

	float Time;
	float Timer;
};

struct Slowmo {};
struct ChargeKill {};
