#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "Light.h"
#include "iw/renderer/Device.h"
#include "iw/common/Components/Transform.h"
#include "iw/util/memory/smart_pointers.h"
#include <unordered_map>
#include <vector>

namespace IW {
namespace Graphics {
	class Renderer {
	public:
		class SceneDataBuilder {
			
		};

		struct SceneData {
			iw::matrix4 World;
			iw::ref<Camera> Camera;
			std::vector<Light> Lights;
			RenderTarget FinalTarget;
			//RenderTarget FinalPostProcessTarget;
		};

		int Width;
		int Height;

		iw::ref<IDevice> Device;
	private:
		Mesh* m_filterMesh;

#ifdef IW_DEBUG
		int lightBeginEndCount = 0;
		int sceneBeginEndCount = 0;
#endif

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
		void SetShader(
			iw::ref<Shader>& shader);

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
		void BeginLight(
			Light* light);

		IWGRAPHICS_API
		void EndLight(
			const Light* light);

		IWGRAPHICS_API
		void CastMesh(
			const Light* light,
			const Transform* transform,
			const Mesh* mesh);

		IWGRAPHICS_API
		void ApplyFilter(
			iw::ref<Shader>& filter,
			RenderTarget* source,
			RenderTarget* dest = nullptr);
	};
}

	using namespace Graphics;
}
