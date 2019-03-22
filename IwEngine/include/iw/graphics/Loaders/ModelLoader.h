#pragma once

#include "iw/util/io/Loader.h"
#include "iw/graphics/Model.h"

namespace IwGraphics {
	class ModelLoader
		: IwUtil::Loader<ModelData>
	{
	public:
		ModelData* Load(
			const char* path);

		void Release(
			ModelData* resource);
	};
}
