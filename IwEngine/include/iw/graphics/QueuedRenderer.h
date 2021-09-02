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
		glm::vec2 m_cameraClipPlane;

		//bool m_reverse;
		setup_draw_func m_beforeDrawFunc; // gets set by 'BeforeDraw' and consumed by 'DrawMesh'
		setup_draw_func m_afterDrawFunc;  // gets set by 'AfterDraw'  and consumed by 'DrawMesh'

		setup_scene_func m_beforeSceneFunc; // gets set by 'BeforeScene' and consumed by 'BeginScene' or 'BeginShadowCast'
		setup_scene_func m_afterSceneFunc;  // gets set by 'AfterScene'  and consumed by 'EndScene'   or 'EndShadowCast'

	public:
		IWGRAPHICS_API
		QueuedRenderer(
			ref<Renderer> renderer);

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
