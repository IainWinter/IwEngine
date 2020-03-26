#include "iw/graphics/QueuedRenderer.h"
#include "iw/events/binding.h"
#include <algorithm>

namespace iw {
namespace Graphics {
	QueuedRenderer::QueuedRenderer(
		const ref<IDevice>& device)
		: Renderer(device)
		, m_pool(500 * (sizeof(BeginSceneOP) + sizeof(BeginShadowOP) + sizeof(DrawMeshOP)))
		, m_layer(0)
		, m_shadow(0)
		, m_camera(0)
		, m_block(0)
		, m_material(0)
		, m_transparency(0)
	{}

	void QueuedRenderer::SetLayer(
		int layer)
	{
		m_layer = layer;
	}

	void QueuedRenderer::Begin() {
		m_queue.emplace_back(0, RenderOP::BEGIN, nullptr);
	}

	void QueuedRenderer::End(){
		m_camera = 0;
		m_queue.emplace_back(-1, RenderOP::END, nullptr);
		
#ifdef IW_DEBUG
		// validate

		bool       invalid = false;
		RenderItem error;

		RenderOP block = RenderOP::NONE;
		for (RenderItem& item : m_queue) {
			switch (block) {
				case RenderOP::BEGIN: {
					switch (item.OP) {
						case RenderOP::BEGIN_SCENE:  block = RenderOP::BEGIN_SCENE;  break;
						case RenderOP::BEGIN_SHADOW: block = RenderOP::BEGIN_SHADOW; break;
						case RenderOP::END:                                          break;
						default:                     invalid = true;                 break;
					}

					break;
				}
				case RenderOP::BEGIN_SCENE: {
					switch (item.OP) {
						case RenderOP::END_SCENE:    block = RenderOP::BEGIN;  break;
						case RenderOP::DRAW_MESH:                              break;
						default:                     invalid = true;           break;
					}

					break;
				}
				case RenderOP::BEGIN_SHADOW: {
					switch (item.OP) {
						case RenderOP::END_SHADOW:   block = RenderOP::BEGIN;  break;
						case RenderOP::DRAW_MESH:                              break;
						default:                     invalid = true;           break;
					}

					break;
				}
				case RenderOP::NONE: {
					break;
				}
				default: {
					invalid = true;
					break;
				}
			}

			if (invalid) {
				error = item;
				break;
			}
		}

		if (invalid) {
			// decode error
			LOG_WARNING << "Error in render queue around <add error info>";
		}
#endif

		// sort

		std::vector<RenderItem> items;
		for (RenderItem& i : m_queue) {
			items.push_back(i);
		}

		std::sort(m_queue.begin(), m_queue.end(), [&](RenderItem& a, RenderItem& b) {
			return a.Order < b.Order;
		});

		std::vector<RenderItem> items2;
		for (RenderItem& i : m_queue) {
			items2.push_back(i);
		}

		// execute

		do {
			RenderItem& item = m_queue.front(); m_queue.pop_front();
			
			switch (item.OP) {
				case RenderOP::BEGIN: {
					Renderer::Begin();
					break;
				}
				case RenderOP::END: {
					Renderer::End();
					break;
				}
				case RenderOP::BEGIN_SCENE: {
					BeginSceneOP* scene = (BeginSceneOP*)item.Data;

					if (scene->Scene) {
						Renderer::BeginScene(scene->Scene, scene->Target);
					}

					else {
						Renderer::BeginScene(scene->Camera, scene->Target);
					}
				
					break;
				}
				case RenderOP::END_SCENE: {
					Renderer::EndScene();
					break;
				}
				case RenderOP::BEGIN_SHADOW: {
					BeginShadowOP* shadow = (BeginShadowOP*)item.Data;

					Renderer::BeginShadowCast(shadow->Light);
					break;					 
				}							 
				case RenderOP::END_SHADOW: {
					Renderer::EndShadowCast();
					break;				
				}						
				case RenderOP::DRAW_MESH: {
					DrawMeshOP* draw = (DrawMeshOP*)item.Data;

					Renderer::DrawMesh(&draw->Transform, draw->Mesh);
					break;
				}
			}

		} while (!m_queue.empty());

		m_pool.reset();
	}

	void QueuedRenderer::BeginScene(
		Camera* camera,
		const ref<RenderTarget>& target)
	{
		m_shadow  = 1;
		m_block   = 1;
		m_camera += 1;
		m_position = camera->Position();

		BeginSceneOP* op = m_pool.alloc<BeginSceneOP>();
		op->Scene  = nullptr;
		op->Camera = camera;
		op->Target = target;

		m_queue.emplace_back(GenOrder(), RenderOP::BEGIN_SCENE, op);
	}

	void QueuedRenderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target)
	{
		m_shadow  = 1;
		m_block   = 1;
		m_camera += 1;
		m_position = scene->MainCamera()->Position();

		BeginSceneOP* op = m_pool.alloc<BeginSceneOP>();
		op->Scene  = scene;
		op->Camera = nullptr;
		op->Target = target;

		m_queue.emplace_back(GenOrder(), RenderOP::BEGIN_SCENE, op);
	}

	void QueuedRenderer::BeginShadowCast(
		Light* light)
	{
		m_shadow  = 0;
		m_block   = 1;
		m_camera += 1;
		m_position = light->ShadowCamera()->Position();

		BeginShadowOP* op = m_pool.alloc<BeginShadowOP>();
		op->Light = light;

		m_queue.emplace_back(GenOrder(), RenderOP::BEGIN_SHADOW, op);
	}

	void QueuedRenderer::EndScene() {
		m_block        = 3;
		m_material     = 0;
		m_transparency = 0;
		
		m_queue.emplace_back(GenOrder(), RenderOP::END_SCENE, nullptr);
	}

	void QueuedRenderer::EndShadowCast() {
		m_block        = 3;
		m_transparency = 0;
		m_material     = 0;
		
		m_queue.emplace_back(GenOrder(), RenderOP::END_SHADOW, nullptr);
	}

	void QueuedRenderer::DrawMesh(
		const Transform* transform,
		Mesh* mesh)
	{
		DrawMesh(*transform, mesh);
	}

	void QueuedRenderer::DrawMesh(
		const Transform& transform,
		Mesh* mesh)
	{
		m_block = 2;
		m_transparency = val(mesh->Material->GetTransparency());
		m_material = mesh->Material->__GetOrder();

		DrawMeshOP* op = m_pool.alloc<DrawMeshOP>();
		op->Transform = transform;
		op->Mesh = mesh;

		m_queue.emplace_back(GenOrder(transform, mesh), RenderOP::DRAW_MESH, op);
	}

	QueuedRenderer::key QueuedRenderer::GenOrder(
		const Transform& transform,
		const Mesh* mesh) const
	{
		key layer        = m_layer;
		key shadow       = m_shadow;
		key camera       = m_camera;
		key block        = m_block;
		key transparency = m_transparency;
		key material     = m_material;
		key depth        = 0;
		
		if (m_camera) {
			if (mesh) {
				depth = 10000 * (m_position - transform.Position).length_fast(); // best value can def be found. Prob some equation of last scenes difference in depth or min value
			}
		}

		if (transparency == val(Transparency::NONE)) {
			layer        <<= val(Bits::LAYER);
			shadow       <<= val(Bits::SHADOW);
			camera       <<= val(Bits::CAMERA);
			block        <<= val(Bits::BLOCK);
			transparency <<= val(Bits::TRANSPARENCY);
			material     <<= val(Bits::MATERIAL);
			depth        <<= val(Bits::DEPTH);
		}

		else {
			layer        <<= val(TransparencyBits::LAYER);
			shadow       <<= val(TransparencyBits::SHADOW);
			camera       <<= val(TransparencyBits::CAMERA);
			block        <<= val(TransparencyBits::BLOCK);
			transparency <<= val(TransparencyBits::TRANSPARENCY);
			depth        <<= val(TransparencyBits::DEPTH);
			material     <<= val(TransparencyBits::MATERIAL);
		}

		return layer | shadow | camera | block | transparency | material | depth;
	}
}
}
