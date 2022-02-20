#pragma once

#include "Renderer.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/util/enum/val.h"
#include <functional>
#include <deque>

namespace iw {
namespace Graphics {
	class QueuedRenderer
	{
	public:
		ref<Renderer> Now;
		ref<IDevice> Device; // = Now->Device

	private:
		using key = uint64_t;
		using setup_draw_func  = std::function<void()>;
		using setup_scene_func = setup_draw_func; // the same rn might be different but probs not

		enum class Bit : key {
			DEPTH        = 8 * sizeof(key) - 64, // 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 xxxx xxxx xxxx xxxx xxxx xxxx
			MATERIAL     = 8 * sizeof(key) - 40, // 0000 0000 0000 0000 0000 0000 xxxx xxxx xxxx xxxx 0000 0000 0000 0000 0000 0000
			TRANSPARENCY = 8 * sizeof(key) - 24, // 0000 0000 0000 0000 0000 00xx 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			BLOCK        = 8 * sizeof(key) - 22, // 0000 0000 0000 0000 0000 xx00 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			CAMERA       = 8 * sizeof(key) - 20, // 0000 0000 xxxx xxxx xxxx 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			SHADOW       = 8 * sizeof(key) - 8,  // 0000 000x 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			LAYER        = 8 * sizeof(key) - 7,  // xxxx xxx0 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
		};

		enum class TBit : key {
			MATERIAL     = 8 * sizeof(key) - 64, // 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 xxxx xxxx xxxx xxxx
			DEPTH        = 8 * sizeof(key) - 48, // 0000 0000 0000 0000 0000 0000 xxxx xxxx xxxx xxxx xxxx xxxx 0000 0000 0000 0000
			TRANSPARENCY = 8 * sizeof(key) - 24, // 0000 0000 0000 0000 0000 00xx 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			BLOCK        = 8 * sizeof(key) - 22, // 0000 0000 0000 0000 0000 xx00 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			CAMERA       = 8 * sizeof(key) - 20, // 0000 0000 xxxx xxxx xxxx 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			SHADOW       = 8 * sizeof(key) - 8,  // 0000 000x 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
			LAYER        = 8 * sizeof(key) - 7,  // xxxx xxx0 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
		};

		enum class BitSize : key {
			DEPTH        = val(Bit::MATERIAL)     - val(Bit::DEPTH),
			MATERIAL     = val(Bit::TRANSPARENCY) - val(Bit::MATERIAL),
			TRANSPARENCY = val(Bit::BLOCK)        - val(Bit::TRANSPARENCY),
			BLOCK        = val(Bit::CAMERA)       - val(Bit::BLOCK),
			CAMERA       = val(Bit::SHADOW)       - val(Bit::CAMERA),
			SHADOW       = val(Bit::LAYER)        - val(Bit::SHADOW),
			LAYER        = 8 * sizeof(key)        - val(Bit::LAYER)
		};

		enum class TBitSize : key {
			MATERIAL     = val(TBit::DEPTH)        - val(TBit::MATERIAL),
			DEPTH        = val(TBit::TRANSPARENCY) - val(TBit::DEPTH),
			TRANSPARENCY = val(TBit::BLOCK)        - val(TBit::TRANSPARENCY),
			BLOCK        = val(TBit::CAMERA)       - val(TBit::BLOCK),
			CAMERA       = val(TBit::SHADOW)       - val(TBit::CAMERA),
			SHADOW       = val(TBit::LAYER)        - val(TBit::SHADOW),
			LAYER        = 8 * sizeof(key)         - val(TBit::LAYER)
		};

		enum class BitMask : key {
			DEPTH        = ((key(1) << val(BitSize::DEPTH       )) - 1) << 0,
			MATERIAL     = ((key(1) << val(BitSize::MATERIAL    )) - 1) << val(Bit::MATERIAL),
			TRANSPARENCY = ((key(1) << val(BitSize::TRANSPARENCY)) - 1) << val(Bit::TRANSPARENCY),
			BLOCK        = ((key(1) << val(BitSize::BLOCK       )) - 1) << val(Bit::BLOCK),
			CAMERA       = ((key(1) << val(BitSize::CAMERA      )) - 1) << val(Bit::CAMERA),
			SHADOW       = ((key(1) << val(BitSize::SHADOW      )) - 1) << val(Bit::SHADOW),
			LAYER        = ((key(1) << val(BitSize::LAYER       )) - 1) << val(Bit::LAYER),
		};

		enum class TBitMask : key {
			MATERIAL     = ((key(1) << val(TBitSize::MATERIAL    )) - 1) << 0,
			DEPTH        = ((key(1) << val(TBitSize::DEPTH       )) - 1) << val(TBit::DEPTH),
			TRANSPARENCY = ((key(1) << val(TBitSize::TRANSPARENCY)) - 1) << val(TBit::TRANSPARENCY),
			BLOCK        = ((key(1) << val(TBitSize::BLOCK       )) - 1) << val(TBit::BLOCK),
			CAMERA       = ((key(1) << val(TBitSize::CAMERA      )) - 1) << val(TBit::CAMERA),
			SHADOW       = ((key(1) << val(TBitSize::SHADOW      )) - 1) << val(TBit::SHADOW),
			LAYER        = ((key(1) << val(TBitSize::LAYER       )) - 1) << val(TBit::LAYER),
		};

		enum class RenderOP {
			NONE,
			BEGIN,        END,
			BEGIN_SCENE,  END_SCENE,
			BEGIN_SHADOW, END_SHADOW,
			DRAW_MESH, DRAW_MESH_INST,
			APPLY_FILTER
		};

		struct BeginSceneOP {
			Scene*  Scene; // if nullptr use camera
			Camera* Camera;
			ref<RenderTarget> Target;
			bool Clear;
			Color ClearColor;

			setup_draw_func BeforeScene;
		};

		struct EndSceneOP {
			setup_draw_func AfterScene;
		};

		struct BeginShadowOP {
			Light* Light;
			bool UseParticleShader;
			bool Clear;

			setup_draw_func BeforeScene;
		};

		struct EndShadowOP {
			setup_draw_func AfterScene;
		};

		struct DrawMeshOP {
			Transform Transform;
			Mesh Mesh;

			setup_draw_func BeforeDraw;
			setup_draw_func AfterDraw;
		};

		struct DrawMeshInstanceOP {
			Transform* Transform;
			Mesh* Mesh;

			setup_draw_func BeforeDraw;
			setup_draw_func AfterDraw;
		};

		struct FilterOP {
			ref<Shader> Filter;
			ref<RenderTarget> Source;
			ref<RenderTarget> Target;
			Camera* Camera;
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

		float m_time;

		int m_layer;
		int m_shadow;
		int m_camera;
		int m_block;
		int m_material;
		int m_transparency;
		glm::vec3 m_position;
		Camera* m_cCamera;

		//bool m_reverse;
		setup_draw_func m_beforeDrawFunc; // gets set by 'BeforeDraw' and consumed by 'DrawMesh'
		setup_draw_func m_afterDrawFunc;  // gets set by 'AfterDraw'  and consumed by 'DrawMesh'

		setup_scene_func m_beforeSceneFunc; // gets set by 'BeforeScene' and consumed by 'BeginScene' or 'BeginShadowCast'
		setup_scene_func m_afterSceneFunc;  // gets set by 'AfterScene'  and consumed by 'EndScene'   or 'EndShadowCast'

	public:
		QueuedRenderer::QueuedRenderer(
			ref<Renderer> renderer
		)
			: Now            (renderer)
			, Device         (Now->Device)
			, m_pool         (16 * 1024, 2)
			, m_layer        (0)
			, m_shadow       (0)
			, m_camera       (0)
			, m_block        (0)
			, m_material     (0)
			, m_transparency (0)
			, m_time         (0)
			, m_position     (0.f)
			, m_cCamera      (nullptr)
		{}

		inline unsigned Width()  { return Now->Width();  }
		inline unsigned Height() { return Now->Height(); }

		// sets current layer for ordering
		IWGRAPHICS_API
		void SetLayer(
			int layer);

		//// sets if distance should be in reverse order
		//IWGRAPHICS_API
		//void SetReverse(
		//	bool reverse);

		// Clears screen buffer
		IWGRAPHICS_API
		void Begin(
			float time = 1.0f);

		// executes queue
		IWGRAPHICS_API
		void End();

		IWGRAPHICS_API
		inline void BeginScene(
			const ref<RenderTarget>& target,
			bool clear = false,
			Color clearColor = Color(0))
		{
			BeginScene((Camera*)nullptr, target, clear, clearColor);
		}

		// set optional camera, identity if null
		// set optional target, screen if null
		IWGRAPHICS_API
		void BeginScene(
			Camera* camera = nullptr,
			const ref<RenderTarget>& target = nullptr,
			bool clear = false,
			Color clearColor = Color(0));

		// calls begin scene
		// set scene lights if provided, no action if null
		IWGRAPHICS_API
		void BeginScene(
			Scene* scene /*= nullptr*/,
			const ref<RenderTarget>& target = nullptr,
			bool clear = false,
			Color clearColor = Color(0));

		// set light camera
		// set light shader
		// set light target
		IWGRAPHICS_API
		void BeginShadowCast(
			Light* light,
			bool useParticleShader = false,
			bool clear = true);

		// marks end of scene, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		void EndScene();

		// marks end of shadow cast, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		void EndShadowCast();

		// Set a function to run before a scene starts,
		//	this is useful for settings that apply to all the draw calls. ie depth testing
		IWGRAPHICS_API
		void BeforeScene(
			const setup_scene_func& func);

		// Set a function to run after a scene ends,
		//	this is useful when settings need to be reset
		IWGRAPHICS_API
		void AfterScene(
			const setup_scene_func& func);

		// Set a function to run before a mesh gets drawn,
		//	this is useful when a mesh needs to be updated in different ways for seperate draw calls
		IWGRAPHICS_API
		void BeforeDraw(
			const setup_draw_func& func);

		// Set a function to run after a mesh gets drawn,
		//	this is useful when settings need to be reset
		IWGRAPHICS_API
		void AfterDraw(
			const setup_draw_func& func);

		// if rendering a scene
		//	set mesh shader
		//	set mesh material
		// if rendering a shadow map
		//	use light shader
		// set transform model matrix
		// set mesh verts and indices
		IWGRAPHICS_API
		void DrawMesh(
			Transform* transform,
			Mesh* mesh);

		IWGRAPHICS_API
		void DrawMesh(
			Transform& transform,
			Mesh& mesh);

		IWGRAPHICS_API
		void ApplyFilter(
			ref<Shader>& filter,
			const ref<RenderTarget>& source,
			const ref<RenderTarget>& target = nullptr,
			Camera* camera = nullptr);
	private:
		key GenOrder(
			Transform* transform = nullptr,
			const Mesh* mesh = nullptr) const;
	};
}

using namespace Graphics;
}
