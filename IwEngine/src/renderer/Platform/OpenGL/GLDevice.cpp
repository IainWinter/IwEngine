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
#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
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
			case POINTS:    glTopology = GL_POINTS;	    break;
			case LINES:     glTopology = GL_LINES;      break;
			case TRIANGLES: glTopology = GL_TRIANGLES;  break;
			case QUADS:     glTopology = GL_QUADS;	    break;

			case LOOP:      glTopology = GL_LINE_LOOP;  break;
			case SEGMENTS:  glTopology = GL_LINE_STRIP; break;

			default: return;
		}

		GL(glDrawElements(
			glTopology,
			(int)count,
			GL_UNSIGNED_INT,
			(const void*)offset));
	}

	void GLDevice::DrawElementsInstanced(
		MeshTopology topology,
		unsigned count,
		ptrdiff_t offset,
		unsigned instanceCount)
	{
		GLenum glTopology = 0;
		switch (topology) {
			case POINTS:    glTopology = GL_POINTS;	   break;
			case LINES:     glTopology = GL_LINES;     break;
			case TRIANGLES: glTopology = GL_TRIANGLES; break;
			case QUADS:     glTopology = GL_QUADS;	   break;
			default: return;
		}

		GL(glDrawElementsInstanced(
			glTopology,
			(int)count,
			GL_UNSIGNED_INT,
			(const void*)offset,
			instanceCount));
	}
	
	void GLDevice::Clear()
	{
		GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void GLDevice::SetClearColor(
		float r, float g, float b, float a)
	{
		GL(glClearColor(r, g, b, a));
	}

	void GLDevice::SetViewport(
		int x, 
		int y)
	{
		GL(glViewport(0, 0, x, y));
	}

	void GLDevice::SetCullFace(
		CullFace cull)
	{
		GL(glCullFace(TRANSLATE(cull)));
	}

	void GLDevice::SetDepthTest(
		bool depthTest)
	{
		if (depthTest) {
			GL(glEnable(GL_DEPTH_TEST));
		}

		else {
			GL(glDisable(GL_DEPTH_TEST));
		}
	}

	void GLDevice::SetWireframe(
		bool wireframe)
	{
		if (wireframe) {
			//glDisable(GL_DEPTH_TEST);
			GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		}

		else {
			//glEnable(GL_DEPTH_TEST);
			GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}
	}

	void GLDevice::DestroyBuffer(
		IBuffer* buffer)
	{
		delete buffer;
	}

	void SetBuffer(
		IBuffer* buffer)
	{
		dynamic_cast<GLBuffer*>(buffer)->Bind();
	}

	void GLDevice::UpdateBuffer(
		IBuffer* buffer, 
		const void* data, 
		size_t size, 
		size_t offset)
	{
		dynamic_cast<GLBuffer*>(buffer)->UpdateData(data, size, offset);
	}

	IIndexBuffer* GLDevice::CreateIndexBuffer(
		const void* data,
		size_t count,
		BufferIOType io)
	{
		return new GLIndexBuffer(data, count, io);
	}

	void GLDevice::SetIndexBuffer(
		IIndexBuffer* indexBuffer)
	{
		if (indexBuffer) {
			SetBuffer(indexBuffer);
		}

		else {
			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); // dont like that this is here. Things are prob backwards tbh
		}
	}

	IVertexBuffer* GLDevice::CreateVertexBuffer(
		const void* data,
		size_t size,
		BufferIOType io)
	{
		return new GLVertexBuffer(data, size, io);
	}

	void GLDevice::SetVertexBuffer(
		IVertexBuffer* vertexBuffer)
	{
		if (vertexBuffer) {
			SetBuffer(vertexBuffer);
		}

		else {
			GL(glBindBuffer(GL_ARRAY_BUFFER, 0)); // dont like that this is here. Things are prob backwards tbh
		}
	}

	IUniformBuffer* GLDevice::CreateUniformBuffer(
		const void* data,
		size_t size,
		BufferIOType io)
	{
		return new GLUniformBuffer(data, size, io);
	}

	void GLDevice::SetUniformBuffer(
		IUniformBuffer* uniformBuffer)
	{
		if (uniformBuffer) {
			SetBuffer(uniformBuffer);
		}

		else {
			GL(glBindBuffer(GL_UNIFORM_BUFFER, 0)); // dont like that this is here. Things are prob backwards tbh
		}
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
			GL(glBindVertexArray(0));
		}
	}

	void GLDevice::AddBufferToVertexArray(
		IVertexArray* vertexArray,
		IVertexBuffer* buffer,
		const VertexBufferLayout& layout,
		int index)
	{
		static_cast<GLVertexArray*>(vertexArray)
			->AddBuffer(static_cast<GLVertexBuffer*>(buffer), layout, index);
	}

	void GLDevice::UpdateVertexArrayData(
		IVertexArray* vertexArray, 
		size_t bufferIndex, 
		const void* data,
		size_t size,
		size_t offset)
	{
		static_cast<GLVertexArray*>(vertexArray)
			->UpdateBuffer(bufferIndex, data, size, offset);
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

	IPipeline* GLDevice::CreateComputePipeline(
		IComputeShader* computeShader)
	{
		return new GLPipeline(
			static_cast<GLComputeShader*>(computeShader));
	}

	void GLDevice::DispatchComputePipeline(
		IPipeline* computePipeline, 
		unsigned x, 
		unsigned y, 
		unsigned z)
	{
		static_cast<GLPipeline*>(computePipeline)
			->DispatchCompute(x, y, z);
	}

	ITexture* GLDevice::CreateTexture(
		int width,
		int height,
		TextureType type,
		TextureFormat format,
		TextureFormatType formatType,
		TextureWrap wrap,
		TextureFilter filter,
		TextureMipmapFilter mipmapFilter,
		void* data)
	{
		return new GLTexture(width, height, type, format, formatType, wrap, filter, mipmapFilter, data);
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

	void GLDevice::SetTextureData(
		const ITexture* texture,
		const ITexture* source,
		int xOffset,
		int yOffset,
		int width,
		int height,
		int mipmap)
	{
		static_cast<const GLTexture*>(texture)
			->SetTextureData(static_cast<const GLTexture*>(source), xOffset, yOffset, width, height, mipmap);
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
			GL(glBindTexture(GL_TEXTURE_2D, 0));
		}
	}

	IFrameBuffer* GLDevice::CreateFrameBuffer(
		bool noColor)
	{
		return new GLFrameBuffer(noColor);
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
			GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		}
	}

	void GLDevice::AttachTextureToFrameBuffer(
		IFrameBuffer* frameBuffer,
		ITexture* texture)
	{
		static_cast<GLFrameBuffer*>(frameBuffer)
			->AttachTexture(static_cast<GLTexture*>(texture));
	}

	
	void GLDevice::ReadPixelsFromFrameBuffer(
		IFrameBuffer* frameBuffer)
	{
		static_cast<GLFrameBuffer*>(frameBuffer)
			->ReadPixels();
	}
}
}
//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
