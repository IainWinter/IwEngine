#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWRENDERER_API __declspec(dllexport)
#else
#		define IWRENDERER_API __declspec(dllimport)
#	endif
#endif

namespace IW {
namespace RenderAPI {
	enum MeshTopology {
		POINTS = 1,
		LINES = 2,
		TRIANGLES = 3,
		QUADS = 4
	};

	enum TextureFormatType {
		UBYTE,
		FLOAT
	};

	// GL has types for every type of value in diffrent bit depths :c but they also dont work so...
	enum TextureFormat {
		ALPHA = 1,
		RG,
		RGB,
		RGBA,
		DEPTH,
		STENCIL
	};
}

	using namespace RenderAPI;
}
