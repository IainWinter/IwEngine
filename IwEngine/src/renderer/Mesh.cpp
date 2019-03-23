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
}
