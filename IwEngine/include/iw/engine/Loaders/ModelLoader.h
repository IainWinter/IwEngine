#pragma once

#include "iw/util/io/Loader.h"
#include "iw/engine/Components/Model.h"
#include "iw/renderer/Device.h"

namespace IwEngine {
	class IWENGINE_API ModelLoader
		: iwu::Loader<ModelData>
	{
	public:
		ModelData* Load(
			const char* path) override;

		void Release(
			ModelData* resource) override;
	};
}