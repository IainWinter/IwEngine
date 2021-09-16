#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWRENDERER_DLL
#		define IWRENDERER_API __declspec(dllexport)
#else
#		define IWRENDERER_API __declspec(dllimport)
#	endif
#else
#	define IWRENDERER_API
#endif

// canmt be gl here
#define TRANSLATE GLTranslator::Instance().Translate

namespace iw {
namespace RenderAPI {
	enum MeshTopology {
		POINTS = 1,
		LINES,
		TRIANGLES,
		QUADS,

		SEGMENTS,
		LOOP
	};

	enum TextureType {
		TEX_2D,
		TEX_3D,
		TEX_CUBE
	};

	enum TextureFormatType {
		UBYTE,
		FLOAT
	};

	enum CullFace {
		FRONT,
		BACK,
		BOTH
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

	enum TextureFilter {
		NEAREST,
		LINEAR
	};

	enum TextureMipmapFilter {
		NEAREST_NEAREST,
		NEAREST_LINEAR,
		LINEAR_NEAREST,
		LINEAR_LINEAR,
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
