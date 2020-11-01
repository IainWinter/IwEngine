#pragma once

#include <string>

enum ItemType {
	NOTE,        // Show a note
	CONSUMABLE,  // Give an item
	ACTION       // Give an ability
};

struct Item {
	ItemType Type;
	unsigned Id;

	std::string SaveState;
};
