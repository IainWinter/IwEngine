#pragma once

#include "IwGraphics.h"
#include "Camera.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/data/Components/Transform.h"
#include "Asset/ModelData.h"
#include "iw/graphics/Mesh.h"
#include <vector>

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Renderer {
	public:
		iwu::ref<IW::IDevice> Device;
		std::vector<iwu::ref<IW::IPipeline>> m_pipelines;
		//std::vector<iwu::ref<IW::IUniformBuffer>> m_uniqueBuffers;
	private:
		IW::IUniformBuffer* m_camera;

	public:
		Renderer(
			const iwu::ref<IW::IDevice>& device);

		iwu::ref<IW::IPipeline>& CreatePipeline(
			const char* vertex,
			const char* fragment);

		iwu::ref<IW::Material> CreateMaterial(
			const iwu::ref<IW::IPipeline>& pipeline);

		iwu::ref<IW::Mesh> CreateMesh(
			const IW::MeshData& data,
			iwu::ref<IW::Material>& material);

		void Begin();
		void End();

		void BeginScene(
			IW::Camera* camera);

		void EndScene();

		void DrawMesh(
			const IW::Transform* transform,
			const IW::Mesh* mesh);
	};
}
}
