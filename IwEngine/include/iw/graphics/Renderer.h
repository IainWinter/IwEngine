#pragma once

#include "IwGraphics.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/data/Components/Transform.h"
#include "iw/graphics/Mesh.h"
#include <vector>

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Renderer {
	public:
		int Width;
		int Height;

		iwu::ref<IW::IDevice> Device;
		std::vector<iwu::ref<IW::IPipeline>> m_pipelines;
		//std::vector<iwu::ref<IW::IUniformBuffer>> m_uniqueBuffers;
	private:
		IW::IUniformBuffer* m_camera;
		IW::Mesh* m_filterMesh;

	public:
		Renderer(
			const iwu::ref<IW::IDevice>& device);

		~Renderer();

		// BADBADBADBAD
		iwu::ref<IW::IPipeline>& CreatePipeline(
			const char* vertex,
			const char* fragment);

		void Initialize();
		void Begin();
		void End();

		void BeginScene(
			IW::Camera* camera,
			IW::RenderTarget* target = nullptr);

		void EndScene();

		void DrawMesh(
			const IW::Transform* transform,
			const IW::Mesh* mesh);

		void ApplyFilter(
			iwu::ref<IW::IPipeline>& pipeline,
			IW::RenderTarget* source,
			IW::RenderTarget* dest = nullptr);
	};
}
}
