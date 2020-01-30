#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWRENDERER_API __declspec(dllexport)
#else
#		define IWRENDERER_API __declspec(dllimport)
#	endif
#endif

// canmt be gl here
#define TRANSLATE GLTranslator::Instance().Translate

namespace IW {
namespace RenderAPI {
	enum MeshTopology {
		POINTS = 1,
		LINES,
		TRIANGLES,
		QUADS
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

	enum TextureWrap {
		EDGE,
		BORDER,
		MIRROR,
		REPEAT,
		MIRROR_EDGE
	};

	enum BufferType {
		VERTEX,
		INDEX,
		UNIFORM
	};

	enum BufferIOType {
		STATIC,
		DYNAMIC
	};
}

	using namespace RenderAPI;
}
