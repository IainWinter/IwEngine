#pragma once

#include "Resource.h"

namespace IwEngine {
	struct Mtl
		: Resource
	{
		Mtl(const char* path)
			: Resource(path)
		{}
	};
}
