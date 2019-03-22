#include "iw/renderer/Mesh.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/math/matrix4.h"
#include "gl/glew.h"

namespace IwRenderer {
	Mesh::Mesh(
		VertexArray* va,
		IndexBuffer* ib)
		: m_vertexArray(va)
		, m_indexBuffer(ib) 
	{}

	Mesh::Mesh(
		const Mesh& copy) noexcept
		: m_vertexArray(copy.m_vertexArray)
		, m_indexBuffer(copy.m_indexBuffer)
	{}

	Mesh::Mesh(
		Mesh&& copy) noexcept
		: m_vertexArray(copy.m_vertexArray)
		, m_indexBuffer(copy.m_indexBuffer)
	{}

	Mesh& Mesh::operator=(
		const Mesh& copy) noexcept
	{
		m_vertexArray = copy.m_vertexArray;
		m_indexBuffer = copy.m_indexBuffer;
		return *this;
	}

	Mesh& Mesh::operator=(
		Mesh&& copy) noexcept
	{
		m_vertexArray = copy.m_vertexArray;
		m_indexBuffer = copy.m_indexBuffer;
		copy.m_vertexArray = nullptr;
		copy.m_indexBuffer = nullptr;
		return *this;
	}

	void Mesh::Draw(
		const iwmath::matrix4& model) const
	{
		iwmath::matrix4 view = iwm::matrix4::identity;

		iwmath::matrix4 projection 
			= iwmath::matrix4::create_perspective_field_of_view(
				1.2f, 1.777f, 0.1f, 1000.0f);

		glUniformMatrix4fv(0, 1, GL_FALSE, model.elements);
		glUniformMatrix4fv(1, 1, GL_FALSE, view.elements);
		glUniformMatrix4fv(2, 1, GL_FALSE, projection.elements);

		m_vertexArray->Bind();
		m_indexBuffer->Bind();

		m_indexBuffer->Draw();
	}
}
