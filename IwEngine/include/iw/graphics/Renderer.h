#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "iw/renderer/Device.h"
#include "iw/common/Components/Transform.h"
#include "iw/util/memory/smart_pointers.h"
#include <vector>

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Renderer {
	public:
		int Width;
		int Height;

		iw::ref<IDevice> Device;
	private:
		Mesh* m_filterMesh;

	public:
		Renderer(
			const iw::ref<IDevice>& device);

		~Renderer();

		void Initialize();
		void Begin();
		void End();

		void BeginScene(
			RenderTarget* target = nullptr);

		void EndScene();

		void DrawMesh(
			const Transform* transform,
			const Mesh* mesh);

		void ApplyFilter(
			iw::ref<IPipeline>& pipeline,
			RenderTarget* source,
			RenderTarget* dest = nullptr);
	};
}
}
