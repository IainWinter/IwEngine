#pragma once

#include "IwGraphics.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "iw/math/vector3.h"
#include "iw/math/quaternion.h"

namespace IwGraphics {
	class IWGRAPHICS_API Mesh {
	private:
		IwGraphics::VertexArray* m_vertexArray;
		IwGraphics::IndexBuffer* m_indexBuffer;

	public:
		Mesh() = default;

		Mesh(
			IwGraphics::VertexArray* va,
			IwGraphics::IndexBuffer* ib);

		Mesh(
			const Mesh& copy);

		Mesh(
			Mesh&& copy);

		~Mesh();

		Mesh& operator=(
			const Mesh& copy);

		Mesh& operator=(
			Mesh&& copy);

		void Draw(
			const iwmath::vector3& position,
			const iwmath::quaternion& rotation) const;
	};
}
