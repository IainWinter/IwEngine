#pragma once

#include "iw/math/vector3.h"

namespace iw {
namespace Graphics {
	struct Vertex {
		iw::vector3 Vertex;
		iw::vector3 Normal;
		//iw::vector2 TextureCoord;
	};
}

	using namespace Graphics;
}
