#pragma once

#include "iw/util/io/Loader.h"
#include "iw/engine/Components/Model.h"
#include "iw/renderer/Device.h"

namespace IwEngine {
	class IWENGINE_API ModelLoader
		: public iwu::Loader<ModelData>
	{
	private:
		ModelData* LoadAsset(
			const char* path) override;
	};
}
