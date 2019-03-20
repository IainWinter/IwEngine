#pragma once

#include "Loader.h"
#include "iw/engine/Resources/Object.h"

namespace IwEngine {
	class ObjLoader
		: Loader<Object>
	{
	public:
		Object* Load(
			const char* path);

		void Release(
			Object* resource);
	};
}
