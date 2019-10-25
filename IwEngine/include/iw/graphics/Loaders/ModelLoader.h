#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Asset/ModelData.h"
#include "iw/util/io/Loader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API ModelLoader
		: public iwu::Loader<ModelData>
	{
	private:
		ModelData* LoadAsset(
			const char* path) override;
	};
}
}

