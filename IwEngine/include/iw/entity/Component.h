#pragma once

#include "IwEntity.h"

namespace IwEntity {
	struct Component {
		size_t  Type;
		size_t  Size;
		const char* Name;
	};
}
