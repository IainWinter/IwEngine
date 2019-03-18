#include "iw/graphics/Mesh.h"
#include "iw/graphics/IndexBuffer.h"
#include "iw/math/matrix4.h"
#include "gl/glew.h"

namespace IwGraphics {
	Mesh::Mesh(
		VertexArray* va,
		IndexBuffer* ib)
		: m_vertexArray(va)
		, m_indexBuffer(ib) 
	{}

	Mesh::~Mesh() {
		delete m_vertexArray;
		delete m_indexBuffer;
	}

	void Mesh::Draw(
		const iwmath::vector3& position,
		const iwmath::quaternion& rotation) const
	{
		iwmath::matrix4 world 
			= iwmath::matrix4::create_from_quaternion(rotation) 
			* iwmath::matrix4::create_translation(position);

		glUniformMatrix4fv(0, 1, GL_FALSE, world.elements);

		m_vertexArray->Bind();
		m_indexBuffer->Bind();

		m_indexBuffer->Draw();
	}
}
