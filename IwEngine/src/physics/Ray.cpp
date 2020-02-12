#include "iw/physics/Ray.h"

namespace iw {
namespace Physics {
namespace impl {
	template<>
	void Ray<iw::vector2>::Transform(
		const iw::Transform& transform)
	{
		Vector /** transform.Rotation */ * transform.Scale + transform.Position;
	}

	template<>
	void Ray<iw::vector3>::Transform(
		const iw::Transform& transform)
	{
		Vector * transform.Rotation * transform.Scale + transform.Position;
	}

	template<>
	void Ray<iw::vector4>::Transform(
		const iw::Transform& transform)
	{
		Vector /** transform.Rotation */ * transform.Scale + transform.Position;
	}
}
}
}
