#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLIndexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "iw/renderer/Platform/OpenGL/GLVertexShader.h"
#include "iw/renderer/Platform/OpenGL/GLFragmentShader.h"
#include "iw/renderer/Platform/OpenGL/GLGeometryShader.h"
#include "iw/renderer/Platform/OpenGL/GLComputeShader.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLComputePipeline.h"
#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "gl/glew.h"

namespace IW {
	IDevice* IDevice::Create() {
		return new GLDevice();
	}

	void GLDevice::DrawElements(
		MeshTopology topology,
		unsigned count, 
		ptrdiff_t offset)
	{
		GLenum glTopology = 0;
		switch (topology) {
			case POINTS:    glTopology = GL_POINTS;	   break;
			case LINES:     glTopology = GL_LINES;     break;
			case TRIANGLES: glTopology = GL_TRIANGLES; break;
			case QUADS:     glTopology = GL_QUADS;	   break;
		}

		if (glTopology) {
			glDrawElements(
				glTopology,
				(int)count,
				GL_UNSIGNED_INT,
				(const void*)offset);
		}
	}

	void GLDevice::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLDevice::SetViewport(
		int x, 
		int y)
	{
		glViewport(0, 0, x, y);
	}

	IIndexBuffer* GLDevice::CreateIndexBuffer(
		size_t size,
		const void* data)
	{
		return new GLIndexBuffer(size, data);
	}

	void GLDevice::DestroyIndexBuffer(
		IIndexBuffer* indexBuffer)
	{
		delete indexBuffer;
	}

	void GLDevice::SetIndexBuffer(
		IIndexBuffer* indexBuffer)
	{
		if (indexBuffer) {
			static_cast<GLIndexBuffer*>(indexBuffer)->Bind();
		}

		else {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	IVertexBuffer* GLDevice::CreateVertexBuffer(
		size_t size,
		const void* data)
	{
		return new GLVertexBuffer(size, data);
	}

	void GLDevice::DestroyVertexBuffer(
		IVertexBuffer* vertexBuffer)
	{
		delete vertexBuffer;
	}

	void GLDevice::SetVertexBuffer(
		IVertexBuffer* vertexBuffer)
	{
		static_cast<GLVertexBuffer*>(vertexBuffer)->Bind();
	}

	void GLDevice::UpdateVertexBufferData(
		IVertexBuffer* vertexBuffer,
		const void* data,
		size_t size)
	{
		static_cast<GLVertexBuffer*>(vertexBuffer)->UpdateData(size, data);
	}

	IUniformBuffer* GLDevice::CreateUniformBuffer(
		size_t size, 
		const void* data)
	{
		return new GLUniformBuffer(size, data);
	}

	void GLDevice::DestroyUniformBuffer(
		IUniformBuffer* uniformBuffer)
	{
		delete uniformBuffer;
	}

	void GLDevice::SetUniformBuffer(
		IUniformBuffer* uniformBuffer)
	{
		static_cast<GLUniformBuffer*>(uniformBuffer)->Bind();
	}

	void GLDevice::UpdateUniformBufferData(
		IUniformBuffer* uniformBuffer,
		const void* data)
	{
		static_cast<GLUniformBuffer*>(uniformBuffer)->UpdateData(data);
	}

	IVertexArray* GLDevice::CreateVertexArray() {
		return new GLVertexArray();
	}

	void GLDevice::DestroyVertexArray(
		IVertexArray* vertexArray) 
	{
		delete vertexArray;
	}

	void GLDevice::SetVertexArray(
		IVertexArray* vertexArray)
	{
		if (vertexArray) {
			static_cast<GLVertexArray*>(vertexArray)->Bind();
		}

		else {
			glBindVertexArray(0);
		}
	}

	void GLDevice::AddBufferToVertexArray(
		IVertexArray* vertexArray,
		IVertexBuffer* buffer,
		const VertexBufferLayout& layout)
	{
		static_cast<GLVertexArray*>(vertexArray)
			->AddBuffer(static_cast<GLVertexBuffer*>(buffer), layout);
	}

	void GLDevice::UpdateVertexArrayData(
		IVertexArray* vertexArray, 
		size_t bufferIndex, 
		const void* data,
		size_t size)
	{
		static_cast<GLVertexArray*>(vertexArray)
			->UpdateBuffer(bufferIndex, size, data);
	}

	IVertexShader* GLDevice::CreateVertexShader(
		const char* source) 
	{
		return new GLVertexShader(source);
	}

	void GLDevice::DestroyVertexArray(
		IVertexShader* vertexShader)
	{
		delete vertexShader;
	}

	IFragmentShader* GLDevice::CreateFragmentShader(
		const char* source)
	{
		return new GLFragmentShader(source);
	}

	void GLDevice::DestroyFragmentShader(
		IFragmentShader* fragmentShader)
	{
		delete fragmentShader;
	}

	IGeometryShader* GLDevice::CreateGeometryShader(
		const char* source)
	{
		return new GLGeometryShader(source);
	}

	void GLDevice::DestroyGeometryShader(
		IGeometryShader* geometryShader)
	{
		delete geometryShader;
	}

	IComputeShader* GLDevice::CreateComputeShader(
		const char* source)
	{
		return new GLComputeShader(source);
	}

	void GLDevice::DestroyComputeShader(
		IComputeShader* computeShader)
	{
		delete computeShader;
	}

	IPipeline* GLDevice::CreatePipeline(
		IVertexShader* vertexShader, 
		IFragmentShader* fragmentShader,
		IGeometryShader* geometryShader)
	{
		return new GLPipeline(
			static_cast<GLVertexShader*>(vertexShader),
			static_cast<GLFragmentShader*>(fragmentShader),
			static_cast<GLGeometryShader*>(geometryShader));
	}

	void GLDevice::DestroyPipeline(
		IPipeline* pipeline)
	{
		delete pipeline;
	}

	void GLDevice::SetPipeline(
		IPipeline* pipeline)
	{
		static_cast<GLPipeline*>(pipeline)->Use();
	}

	IComputePipeline* GLDevice::CreateComputePipeline(
		IComputeShader* computeShader)
	{
		return new GLComputePipeline(
			static_cast<GLComputeShader*>(computeShader));
	}

	void GLDevice::DestroyComputePipeline(
		IComputePipeline* computePipeline)
	{
		delete computePipeline;
	}

	void GLDevice::SetComputePipeline(
		IComputePipeline* computePipeline)
	{
		static_cast<GLComputePipeline*>(computePipeline)->Use();
	}

	ITexture* GLDevice::CreateTexture(
		int width, 
		int height, 
		TextureFormat format, 
		TextureFormatType type,
		const void* data)
	{
		return new GLTexture(width, height, format, type, data);
	}

	ITexture* GLDevice::CreateSubTexture(
		ITexture* texture,
		int xOffset,
		int yOffset,
		int width,
		int height,
		int mipmap)
	{
		return static_cast<GLTexture*>(texture)
			->CreateSubTexture(xOffset, yOffset, width, height, mipmap);
	}

	void GLDevice::DestroyTexture(
		ITexture* texture)
	{
		delete texture;
	}

	void GLDevice::SetTexture(
		ITexture* texture)
	{
		if (texture) {
			static_cast<GLTexture*>(texture)->Bind();
		}

		else {
			glBindTexture(GL_TEXTURE_2D, 0); // this is prob the way to do it cus you cant call unbind a nullptr
		}
	}

	IFrameBuffer* GLDevice::CreateFrameBuffer() {
		return new GLFrameBuffer();
	}

	void GLDevice::DestroyFrameBuffer(
		IFrameBuffer* frameBuffer)
	{
		delete frameBuffer;
	}

	void GLDevice::SetFrameBuffer(
		IFrameBuffer* frameBuffer)
	{
		if (frameBuffer) {
			static_cast<GLFrameBuffer*>(frameBuffer)->Bind();
		}

		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);  // this is prob the way to do it cus you cant call unbind a nullptr
		}
	}

	void GLDevice::AttachTextureToFrameBuffer(
		IFrameBuffer* frameBuffer,
		ITexture* texture)
	{
		static_cast<GLFrameBuffer*>(frameBuffer)
			->AttachTexture(static_cast<GLTexture*>(texture));
	}
}

//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
