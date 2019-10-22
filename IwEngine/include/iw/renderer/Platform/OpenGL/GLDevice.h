#pragma once

#include "iw/renderer/Device.h"

namespace IwRenderer {
	class IWRENDERER_API GLDevice
		: public IDevice
	{
	public:
		void DrawElements(
			MeshTopology topology,
			int count,
			long long offset) override;

		//Index buffers
		IIndexBuffer* CreateIndexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyIndexBuffer(
			IIndexBuffer* indexBuffer) override;

		void SetIndexBuffer(
			IIndexBuffer* indexBuffer) override;

		//Vertex buffers
		IVertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyVertexBuffer(
			IVertexBuffer* vertexBuffer) override;

		void SetVertexBuffer(
			IVertexBuffer* vertexBuffer) override;

		//Vertex arrays
		IVertexArray* CreateVertexArray() override;

		void DestroyVertexArray(
			IVertexArray* vertexArray) override;

		void SetVertexArray(
			IVertexArray* vertexArray) override;

		//Vertex shader
		IVertexShader* CreateVertexShader(
			const char* source) override;

		void DestroyVertexArray(
			IVertexShader* vertexShader) override;

		//Fragment shader
		IFragmentShader* CreateFragmentShader(
			const char* source) override;

		void DestroyFragmentShader(
			IFragmentShader* fragmentShader) override;

		//Geometry shader
		IGeometryShader* CreateGeometryShader(
			const char* source) override;

		void DestroyGeometryShader(
			IGeometryShader* geometryShader) override;

		//Compute shader
		virtual IComputeShader* CreateComputeShader(
			const char* source) override;

		virtual void DestroyComputeShader(
			IComputeShader* computeShader) override;

		//Shader pipeline
		IPipeline* CreatePipeline(
			IVertexShader* vertexShader,
			IFragmentShader* fragmentShader,
			IGeometryShader* geometryShader) override;

		void DestroyPipeline(
			IPipeline* pipeline) override;

		void SetPipeline(
			IPipeline* pipeline) override;

		//Compute shader pipeline
		virtual IComputePipeline* CreateComputePipeline(
			IComputeShader* computeShader) override;

		virtual void DestroyComputePipeline(
			IComputePipeline* computePipeline) override;

		virtual void SetComputePipeline(
			IComputePipeline* ComputePipeline) override;
	};
}
