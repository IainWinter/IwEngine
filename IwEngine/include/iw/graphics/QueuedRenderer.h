#pragma once

#include "Renderer.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/util/enum/val.h"
#include <deque>

namespace iw {
namespace Graphics {
	class QueuedRenderer
		: public Renderer
	{
	private:
		enum class Bits {
			LAYER        = 0,
			CAMERA       = 3,
			TRANSPARENCY = 6,
			DEPTH        = 8,
			MATERIAL     = 32,
		};

		enum class TransparencyBits {
			LAYER = 0,
			CAMERA = 3,
			TRANSPARENCY = 6,
			MATERIAL = 8,
			DEPTH = 32
		};

		enum class RenderOP {
			NONE,
			BEGIN,        END,
			BEGIN_SCENE,  END_SCENE,
			BEGIN_SHADOW, END_SHADOW,
			DRAW_MESH
		};

		struct BeginSceneOP {
			Scene* Scene;
			Camera* Camera; // if nullptr use camera
			ref<RenderTarget> Target;
		};

		struct BeginShadowOP {
			Light* Light;
		};

		struct DrawMeshOP {
			const Transform* Transform;
			const Mesh*      Mesh;
		};

		struct RenderItem {
			int64_t  Order;
			RenderOP OP;
			void*    Data;

			RenderItem()
				: Order(0)
				, OP(RenderOP::NONE)
				, Data(nullptr)
			{}

			RenderItem(
				int64_t order,
				RenderOP op,
				void* data)
				: Order(order)
				, OP(op)
				, Data(data)
			{}
		};

		std::deque<RenderItem> m_queue;
		pool_allocator m_pool;

	public:
		IWGRAPHICS_API
		QueuedRenderer(
		const ref<IDevice>& device);

		// Clears screen buffer
		IWGRAPHICS_API
		void Begin() override;

		// executes queue
		IWGRAPHICS_API
		void End() override;

		// set optional camera, identity if null
		// set optional target, screen if null
		IWGRAPHICS_API
		void BeginScene(
			Camera* camera = nullptr,
			const ref<RenderTarget>& target = nullptr) override;

		// calls begin scene
		// set scene lights if provided, no action if null
		IWGRAPHICS_API
		void BeginScene(
			Scene* scene = nullptr,
			const ref<RenderTarget>& target = nullptr) override;

		// set light camera
		// set light shader
		// set light target
		IWGRAPHICS_API
		void BeginShadowCast(
			Light* light) override;

		// marks end of scene, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		void EndScene() override;

		// marks end of shadow cast, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		void EndShadowCast() override;

		// if rendering a scene
		//	set mesh shader
		//	set mesh material
		// if rendering a shadow map
		//	use light shader
		// set transform model matrix
		// set mesh verts and indices
		IWGRAPHICS_API
		void DrawMesh(
			const Transform* transform,
			const Mesh* mesh) override;
		private:
			bool Sort(
				RenderItem& a,
				RenderItem& b);

			int GetBits(
				int64_t order,
				int k, int p);

			int GetOrder(
				int64_t order,
				Bits bits);
	};
}

using namespace Graphics;
}
