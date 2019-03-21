#pragma once

#include "Loader.h"
#include "iw/engine/Resources/Model.h"

namespace IwEngine {
	class ModelLoader
		: Loader<Model>
	{
	public:
		Model* Load(
			const char* path);

		void Release(
			Model* resource);
	};
}
