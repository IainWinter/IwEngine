#pragma once

#include "Resource.h"

namespace IwEngine {
	struct Material
		: Resource
	{
		Material(const char* path)
			: Resource(path)
		{}
	};
}
