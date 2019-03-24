#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLIndexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "iw/renderer/Platform/OpenGL/GLVertexShader.h"
#include "iw/renderer/Platform/OpenGL/GLFragmentShader.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "gl/glew.h"

namespace IwRenderer {
	void GLDevice::DrawElements(
		int count, 
		long long offset)
	{
		glDrawElements(
			GL_TRIANGLES, 
			count, 
			GL_UNSIGNED_INT, 
			reinterpret_cast<const void*>(offset));
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

	void GLDevice::SetIndexBuffer(
		IndexBuffer* indexBuffer)
	{
		static_cast<GLIndexBuffer*>(indexBuffer)->Bind();
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

	void GLDevice::SetVertexBuffer(
		VertexBuffer* vertexBuffer)
	{
		static_cast<GLVertexBuffer*>(vertexBuffer)->Bind();
	}

	VertexArray* GLDevice::CreateVertexArray(
		size_t numBuffers,
		VertexBuffer** vertexBuffers,
		VertexBufferLayout** vertexLayouts)
	{
		GLVertexArray* va = new GLVertexArray();
		for (size_t i = 0; i < numBuffers; i++) {
			va->AddBuffer(
				static_cast<GLVertexBuffer*>(vertexBuffers[i]),
				vertexLayouts[i]);
		}

		return va;
	}

	void GLDevice::DestroyVertexArray(
		VertexArray* vertexArray) 
	{
		delete vertexArray;
	}

	void GLDevice::SetVertexArray(
		VertexArray* vertexArray) 
	{
		static_cast<GLVertexArray*>(vertexArray)->Bind();
	}

	VertexShader* GLDevice::CreateVertexShader(
		const char* source) 
	{
		return new GLVertexShader(source);
	}

	void GLDevice::DestroyVertexArray(
		VertexShader* vertexShader)
	{
		delete vertexShader;
	}

	FragmentShader* GLDevice::CreateFragmentShader(
		const char* source)
	{
		return new GLFragmentShader(source);
	}

	void GLDevice::DestroyFragmentShader(
		FragmentShader* fragmentShader)
	{
		delete fragmentShader;
	}

	Pipeline* GLDevice::CreatePipeline(
		VertexShader* vertexShader, 
		FragmentShader* fragmentShader)
	{
		return new GLPipeline(
			static_cast<GLVertexShader*>(vertexShader),
			static_cast<GLFragmentShader*>(fragmentShader));
	}

	void GLDevice::DestroyPipeline(
		Pipeline* pipeline)
	{
		delete pipeline;
	}

	void GLDevice::SetPipeline(
		Pipeline* pipeline)
	{
		static_cast<GLPipeline*>(pipeline)->Use();
	}
}
