#pragma once

#include "VertexArray.h" //TODO: Put in /src and acces through thread safe system (same goes for all \graphics\... .h)
#include "IndexBuffer.h"
#include "VertexBufferLayout.h" //TODO: Not this one though because it is thread safe
#include "iw/math/vector3.h"
#include "iw/math/quaternion.h"

namespace IwGraphics {
	class Mesh {
	private:
		IwGraphics::VertexArray* m_vertexArray;
		IwGraphics::IndexBuffer* m_indexBuffer;

	public:
		Mesh() = default;

		Mesh(
			IwGraphics::VertexArray* va,
			IwGraphics::IndexBuffer* ib);

		~Mesh();

		void Draw(
			const iwmath::vector3& position,
			const iwmath::quaternion& rotation) const;
	};
}
