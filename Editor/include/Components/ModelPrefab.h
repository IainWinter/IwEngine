#pragma once

#include "iw/common/Components/Transform.h"
#include <string>

struct REFLECT ModelPrefab {
	REFLECT std::string ModelName;
	REFLECT iw::Transform Transform;
};
