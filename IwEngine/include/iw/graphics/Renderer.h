#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "iw/renderer/Device.h"
#include "iw/common/Components/Transform.h"
#include "iw/util/memory/smart_pointers.h"
#include <vector>

namespace IW {
namespace Graphics {
	class Renderer {
	public:
		int Width;
		int Height;

		iw::ref<IDevice> Device;
	private:
		Mesh* m_filterMesh;

	public:
		IWGRAPHICS_API
		Renderer(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		~Renderer();

		IWGRAPHICS_API
		void Initialize();

		IWGRAPHICS_API
		void Begin();

		IWGRAPHICS_API
		void End();

		IWGRAPHICS_API
		void BeginScene(
			RenderTarget* target = nullptr);

		IWGRAPHICS_API
		void EndScene();

		IWGRAPHICS_API
		void DrawMesh(
			const Transform* transform,
			const Mesh* mesh);

		IWGRAPHICS_API
		void ApplyFilter(
			iw::ref<IPipeline>& pipeline,
			RenderTarget* source,
			RenderTarget* dest = nullptr);
	};
}

	using namespace Graphics;
}
