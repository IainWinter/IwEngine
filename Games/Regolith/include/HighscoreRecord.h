#pragma once

#include <string>
#include "iw/util/reflection/ReflectDef.h"

struct REFLECT HighscoreRecord
{
	std::string GameId;
	std::string Name;
	int Score;
	int Order;
};
