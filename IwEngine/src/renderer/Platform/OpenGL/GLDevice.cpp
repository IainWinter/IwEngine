#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLIndexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "gl/glew.h"

#include "iw/math/matrix4.h"

namespace IwRenderer {
	void GLDevice::DrawElements(
		int count, 
		long long offset)
	{
		iwmath::matrix4 model = iwm::matrix4::create_translation(0, -2.5f, -10);

		iwmath::matrix4 view = iwm::matrix4::identity;

		iwmath::matrix4 projection
			= iwmath::matrix4::create_perspective_field_of_view(
				1.2f, 1.777f, 0.1f, 1000.0f);

		glUniformMatrix4fv(0, 1, GL_FALSE, model.elements);
		glUniformMatrix4fv(1, 1, GL_FALSE, view.elements);
		glUniformMatrix4fv(2, 1, GL_FALSE, projection.elements);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(offset));
	}

	IndexBuffer* GLDevice::CreateIndexBuffer(
		size_t size,
		const void* data)
	{
		return new GLIndexBuffer(size, data);
	}

	void GLDevice::DestroyIndexBuffer(
		IndexBuffer* indexBuffer)
	{
		delete indexBuffer;
	}

	Device* GLDevice::SetIndexBuffer(
		IndexBuffer* indexBuffer)
	{
		static_cast<GLIndexBuffer*>(indexBuffer)->Bind();
		return this;
	}

	VertexBuffer* GLDevice::CreateVertexBuffer(
		size_t size,
		const void* data)
	{
		return new GLVertexBuffer(size, data);
	}

	void GLDevice::DestroyVertexBuffer(
		VertexBuffer* vertexBuffer)
	{
		delete vertexBuffer;
	}

	Device* GLDevice::SetVertexBuffer(
		VertexBuffer* vertexBuffer)
	{
		static_cast<GLVertexBuffer*>(vertexBuffer)->Bind();
		return this;
	}

	VertexArray* GLDevice::CreateVertexArray(
		size_t numBuffers,
		VertexBuffer** vertexBuffers,
		VertexBufferLayout** vertexLayouts)
	{
		GLVertexArray* va = new GLVertexArray();
		for (size_t i = 0; i < numBuffers; i++) {
			va->AddBuffer(static_cast<GLVertexBuffer*>(vertexBuffers[i]),
				vertexLayouts[i]);
		}

		return va;
	}

	void GLDevice::DestroyVertexArray(
		VertexArray* vertexArray) 
	{
		delete vertexArray;
	}

	Device* GLDevice::SetVertexArray(
		VertexArray* vertexArray) 
	{
		static_cast<GLVertexArray*>(vertexArray)->Bind();
		return this;
	}
}
