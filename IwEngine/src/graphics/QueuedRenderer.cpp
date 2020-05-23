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
				case RenderOP::NONE:
				case RenderOP::BEGIN: {
					switch (item.OP) {
						case RenderOP::BEGIN_SCENE:  block = RenderOP::BEGIN_SCENE;  break;
						case RenderOP::BEGIN_SHADOW: block = RenderOP::BEGIN_SHADOW; break;
						case RenderOP::BEGIN:                                        break;
						case RenderOP::END:                                          break;
						default:                     invalid = true;                 break;
					}

					break;
				}
				case RenderOP::BEGIN_SCENE: {
					switch (item.OP) {
						case RenderOP::END_SCENE:    block = RenderOP::NONE; break;
						case RenderOP::DRAW_MESH:                            break;
						case RenderOP::DRAW_MESH_INST:                       break;
						default:                     invalid = true;         break;
					}

					break;
				}
				case RenderOP::BEGIN_SHADOW: {
					switch (item.OP) {
						case RenderOP::END_SHADOW:   block = RenderOP::NONE; break;
						case RenderOP::DRAW_MESH:                            break;
						case RenderOP::DRAW_MESH_INST:                       break;
						default:                     invalid = true;         break;
					}

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

		std::sort(m_queue.begin(), m_queue.end(), [&](RenderItem& a, RenderItem& b) {
			return a.Order < b.Order;
		});

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
						Renderer::BeginScene(scene->Scene, scene->Target, scene->Clear);
					}

					else {
						Renderer::BeginScene(scene->Camera, scene->Target, scene->Clear);
					}
				
					break;
				}
				case RenderOP::END_SCENE: {
					Renderer::EndScene();
					break;
				}
				case RenderOP::BEGIN_SHADOW: {
					BeginShadowOP* shadow = (BeginShadowOP*)item.Data;

					Renderer::BeginShadowCast(shadow->Light, shadow->UseParticleShader, shadow->Clear);
					break;					 
				}							 
				case RenderOP::END_SHADOW: {
					Renderer::EndShadowCast();
					break;				
				}						
				case RenderOP::DRAW_MESH: {
					DrawMeshOP* draw = (DrawMeshOP*)item.Data;

					if (draw->BeforeDraw) {
						draw->BeforeDraw();
					}

					Renderer::DrawMesh(draw->Transform, draw->Mesh);

					if (draw->AfterDraw) {
						draw->AfterDraw();
					}

					m_pool.free<DrawMeshOP>(draw);
					break;
				}
				case RenderOP::DRAW_MESH_INST: {
					DrawMeshInstanceOP* draw = (DrawMeshInstanceOP*)item.Data;

					if (draw->BeforeDraw) {
						draw->BeforeDraw();
					}

					Renderer::DrawMesh(draw->Transform, draw->Mesh);

					if (draw->AfterDraw) {
						draw->AfterDraw();
					}

					m_pool.free<DrawMeshInstanceOP>(draw);
					break;
				}
				case RenderOP::APPLY_FILTER: {
					FilterOP* filter = (FilterOP*)item.Data;

					Renderer::ApplyFilter(filter->Filter, filter->Source, filter->Target);

					m_pool.free<FilterOP>(filter);
					break;
				}
			}

		} while (!m_queue.empty());

		m_pool.reset();
	}

	void QueuedRenderer::BeginScene(
		Camera* camera,
		const ref<RenderTarget>& target,
		bool clear)
	{
		m_shadow  = 1;
		m_block   = 1;
		m_camera += 1;
		m_position = camera ? camera->Position() : vector3::zero;

		BeginSceneOP* op = m_pool.alloc<BeginSceneOP>();
		op->Scene  = nullptr;
		op->Camera = camera;
		op->Target = target;
		op->Clear  = clear;

		m_queue.emplace_back(GenOrder(), RenderOP::BEGIN_SCENE, op);
	}

	void QueuedRenderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target,
		bool clear)
	{
		m_shadow  = 1;
		m_block   = 1;
		m_camera += 1;
		m_position = scene->MainCamera()->Position();

		BeginSceneOP* op = m_pool.alloc<BeginSceneOP>();
		op->Scene  = scene;
		op->Camera = nullptr;
		op->Target = target;
		op->Clear  = clear;

		m_queue.emplace_back(GenOrder(), RenderOP::BEGIN_SCENE, op);
	}

	void QueuedRenderer::BeginShadowCast(
		Light* light,
		bool useParticleShader,
		bool clear)
	{
		m_shadow  = 0;
		m_block   = 1;
		m_camera += 1;
		m_position = light->Position();

		BeginShadowOP* op = m_pool.alloc<BeginShadowOP>();
		op->Light = light;
		op->UseParticleShader = useParticleShader;
		op->Clear = clear;

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
		m_material     = 0;
		m_transparency = 0;
		
		m_queue.emplace_back(GenOrder(), RenderOP::END_SHADOW, nullptr);
	}

	void QueuedRenderer::BeforeDraw(
		const setup_draw_func& func)
	{
		m_beforeDrawFunc = func;
	}

	void QueuedRenderer::AfterDraw(
		const setup_draw_func& func)
	{
		m_afterDrawFunc = func;
	}

	void QueuedRenderer::DrawMesh(
		const Transform* transform,
		Mesh* mesh)
	{
		m_block        = 2;
		m_material     =     mesh->Material() ? mesh->Material()->__GetOrder()   : 0;
		m_transparency = val(mesh->Material() ? mesh->Material()->Transparency() : Transparency::NONE);

		DrawMeshInstanceOP* op = m_pool.alloc<DrawMeshInstanceOP>();
		op->Transform = transform;
		op->Mesh      = mesh;
		op->BeforeDraw = m_beforeDrawFunc;
		op->AfterDraw  = m_afterDrawFunc;

		m_beforeDrawFunc = nullptr; // reset for next call
		m_afterDrawFunc  = nullptr; // reset for next call

		m_queue.emplace_back(GenOrder(transform, mesh), RenderOP::DRAW_MESH_INST, op);
	}

	void QueuedRenderer::DrawMesh(
		const Transform& transform,
		Mesh& mesh)
	{
		m_block        = 2;
		m_material     =     mesh.Material() ? mesh.Material()->__GetOrder()   : 0;
		m_transparency = val(mesh.Material() ? mesh.Material()->Transparency() : Transparency::NONE);

		DrawMeshOP* op = m_pool.alloc<DrawMeshOP>();
		op->Transform = transform;
		op->Mesh      = mesh;
		op->BeforeDraw = m_beforeDrawFunc;
		op->AfterDraw  = m_afterDrawFunc;

		m_beforeDrawFunc = nullptr; // reset for next call
		m_afterDrawFunc = nullptr; // reset for next call

		m_queue.emplace_back(GenOrder(&transform, &mesh), RenderOP::DRAW_MESH, op);
	}

	void QueuedRenderer::ApplyFilter(
		ref<Shader>& filter,
		const ref<RenderTarget>& source,
		const ref<RenderTarget>& target)
	{
		m_block        = 0;
		m_material     = 0;
		m_transparency = 0;
		m_camera       = 0; // find which camera was used to render to the source target and put after that

		FilterOP* op = m_pool.alloc<FilterOP>();
		op->Filter = filter;
		op->Source = source;
		op->Target = target;

		m_queue.emplace_back(GenOrder(), RenderOP::APPLY_FILTER, op);
	}

	QueuedRenderer::key QueuedRenderer::GenOrder(
		const Transform* transform,
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
				depth = 10000.0f * (m_position - (transform ? transform->Position : 0)).length_fast();
			}
		}

		if (transparency == val(Transparency::NONE)) {
			layer        <<= val(Bits::LAYER);
			shadow       <<= val(Bits::SHADOW);
			camera       <<= val(Bits::CAMERA);
			block        <<= val(Bits::BLOCK);
			transparency <<= val(Bits::TRANSPARENCY);
			material     <<= val(Bits::MATERIAL);

			depth          = (((key)1 << val(Bits::MATERIAL)) - 1) & ((key)-1 - depth);
		}

		else {
			layer        <<= val(TransparencyBits::LAYER);
			shadow       <<= val(TransparencyBits::SHADOW);
			camera       <<= val(TransparencyBits::CAMERA);
			block        <<= val(TransparencyBits::BLOCK);
			transparency <<= val(TransparencyBits::TRANSPARENCY);
			material     <<= val(TransparencyBits::MATERIAL);

			depth         = (((key)1 << val(TransparencyBits::DEPTH)) - 1) & ((key)-1 - depth);

		}

		return layer | shadow | camera | block | transparency | material | depth;
	}
}
}
