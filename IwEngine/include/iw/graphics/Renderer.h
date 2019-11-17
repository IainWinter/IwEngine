#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "iw/renderer/Device.h"
#include "iw/data/Components/Transform.h"
#include "iw/util/memory/smart_pointers.h"
#include <vector>

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Renderer {
	public:
		int Width;
		int Height;

		iw::ref<IW::IDevice> Device;
	private:
		IW::Mesh* m_filterMesh;

	public:
		Renderer(
			const iw::ref<IW::IDevice>& device);

		~Renderer();

		void Initialize();
		void Begin();
		void End();

		void BeginScene(
			IW::RenderTarget* target = nullptr);

		void EndScene();

		void DrawMesh(
			const IW::Transform* transform,
			const IW::Mesh* mesh);

		void ApplyFilter(
			iw::ref<IW::IPipeline>& pipeline,
			IW::RenderTarget* source,
			IW::RenderTarget* dest = nullptr);
	};
}
}
