#include "iw/physics/Ray.h"

namespace iw {
namespace Physics {
namespace impl {
	template<>
	void Ray<glm::vec3>::Transform(
		const iw::Transform& transform)
	{
		Vector * transform.Rotation * transform.Scale + transform.Position;
	}
}
}
}
