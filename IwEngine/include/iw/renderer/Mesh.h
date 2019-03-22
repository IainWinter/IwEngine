#pragma once

#include "IwRenderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "iw/math/matrix4.h"
#include <memory>

namespace IwRenderer {
	class IWRENDERER_API Mesh {
	private:
		VertexArray* m_vertexArray;
		IndexBuffer* m_indexBuffer;

	public:
		Mesh() = default;

		Mesh(
			VertexArray* va,
			IndexBuffer* ib);

		Mesh(
			const Mesh& copy) noexcept;

		Mesh(
			Mesh&& copy) noexcept;

		Mesh& operator=(
			const Mesh& copy) noexcept;

		Mesh& operator=(
			Mesh&& copy) noexcept;

		void Draw(
			const iwmath::matrix4& model) const;
	};
}
