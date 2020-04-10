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
		using key = uint64_t;

		enum class Bits {
			DEPTH        = 8 * sizeof(key) - 64, // 0000000000000000000000000000000000000000xxxxxxxxxxxxxxxxxxxxxxxx
			MATERIAL     = 8 * sizeof(key) - 40, // 000000000000000000000000xxxxxxxxxxxxxxxx000000000000000000000000
			TRANSPARENCY = 8 * sizeof(key) - 24, // 0000000000000000000000xx0000000000000000000000000000000000000000
			BLOCK        = 8 * sizeof(key) - 22, // 00000000000000000000xx000000000000000000000000000000000000000000
			CAMERA       = 8 * sizeof(key) - 20, // 00000000xxxxxxxxxxxx00000000000000000000000000000000000000000000
			SHADOW       = 8 * sizeof(key) - 8,  // 0000000x00000000000000000000000000000000000000000000000000000000
			LAYER        = 8 * sizeof(key) - 7,  // xxxxxxx000000000000000000000000000000000000000000000000000000000
		};

		enum class TransparencyBits {
			MATERIAL     = 8 * sizeof(key) - 64, // 000000000000000000000000000000000000000000000000xxxxxxxxxxxxxxxx
			DEPTH        = 8 * sizeof(key) - 48, // 000000000000000000000000xxxxxxxxxxxxxxxxxxxxxxxx0000000000000000
			TRANSPARENCY = 8 * sizeof(key) - 24, // 0000000000000000000000xx0000000000000000000000000000000000000000
			BLOCK        = 8 * sizeof(key) - 22, // 00000000000000000000xx000000000000000000000000000000000000000000
			CAMERA       = 8 * sizeof(key) - 20, // 00000000xxxxxxxxxxxx00000000000000000000000000000000000000000000
			SHADOW       = 8 * sizeof(key) - 8,  // 0000000x00000000000000000000000000000000000000000000000000000000
			LAYER        = 8 * sizeof(key) - 7,  // xxxxxxx000000000000000000000000000000000000000000000000000000000
		};

		enum class RenderOP {
			NONE,
			BEGIN,        END,
			BEGIN_SCENE,  END_SCENE,
			BEGIN_SHADOW, END_SHADOW,
			DRAW_MESH,
			APPLY_FILTER
		};

		struct BeginSceneOP {
			Scene*  Scene; // if nullptr use camera
			Camera* Camera;
			ref<RenderTarget> Target;
		};

		struct BeginShadowOP {
			Light* Light;
		};

		struct DrawMeshOP {
			Transform Transform; // see End function
			Mesh* Mesh;
		};

		struct FilterOP {
			ref<Shader> Filter;
			ref<RenderTarget> Source;
			ref<RenderTarget> Target;
		};

		struct RenderItem {
			uint64_t Order;
			RenderOP OP;
			void* Data;

			RenderItem()
				: Order(0)
				, OP(RenderOP::NONE)
				, Data(nullptr)
			{}

			RenderItem(
				uint64_t order,
				RenderOP op,
				void* data)
				: Order(order)
				, OP(op)
				, Data(data)
			{}
		};

		std::deque<RenderItem> m_queue;
		pool_allocator m_pool;

		int m_layer;
		int m_shadow;
		int m_camera;
		int m_block;
		int m_material;
		int m_transparency;
		vector3 m_position;

	public:
		IWGRAPHICS_API
		QueuedRenderer(
		const ref<IDevice>& device);

		// sets current layer for ordering
		IWGRAPHICS_API
		void SetLayer(
			int layer);

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
			Mesh* mesh) override;

		// Copies transform to be able to use stack allocated ones. DrawMesh(*, *) calls to this. TODO: could make 2 differnt draw calls one for instances and one for ptrs
		IWGRAPHICS_API
		void DrawMesh(
			const Transform& transform,
			Mesh* mesh);

		IWGRAPHICS_API
		void ApplyFilter(
			ref<Shader>& filter,
			const ref<RenderTarget>& source,
			const ref<RenderTarget>& target = nullptr) override;
	private:
		key GenOrder(
			const Transform& transform = Transform(),
			const Mesh* mesh = nullptr) const;
	};
}

using namespace Graphics;
}
