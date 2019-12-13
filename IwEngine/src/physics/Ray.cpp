#include "iw/physics/Ray.h"

namespace IW {
namespace Physics {
namespace impl {
	template<>
	void Ray<iw::vector2>::Transform(
		const IW::Transform& transform)
	{
		Vector /** transform.Rotation */ * transform.Scale + transform.Position;
	}

	template<>
	void Ray<iw::vector3>::Transform(
		const IW::Transform& transform)
	{
		Vector * transform.Rotation * transform.Scale + transform.Position;
	}

	template<>
	void Ray<iw::vector4>::Transform(
		const IW::Transform& transform)
	{
		Vector /** transform.Rotation */ * transform.Scale + transform.Position;
	}
}
}
}
