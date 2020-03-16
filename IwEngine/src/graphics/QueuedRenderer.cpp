#include "iw/graphics/QueuedRenderer.h"
#include "iw/events/binding.h"
#include <algorithm>

namespace iw {
namespace Graphics {
	QueuedRenderer::QueuedRenderer(
		const ref<IDevice>& device)
		: Renderer(device)
		, m_pool(500 * (sizeof(BeginSceneOP) + sizeof(BeginShadowOP) + sizeof(DrawMeshOP)))
	{}

	void QueuedRenderer::Begin() {
		m_queue.emplace_back(0, RenderOP::BEGIN, nullptr);
	}

	void QueuedRenderer::End(){
		m_queue.emplace_back(0, RenderOP::END, nullptr);
		
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
						case RenderOP::END_SCENE:    block = RenderOP::BEGIN_SCENE;  break;
						case RenderOP::DRAW_MESH:                                    break;
						default:                     invalid = true;                 break;
					}

					break;
				}
				case RenderOP::BEGIN_SHADOW: {
					switch (item.OP) {
						case RenderOP::END_SHADOW:   block = RenderOP::BEGIN_SCENE;  break;
						case RenderOP::DRAW_MESH:                                    break;
						default:                     invalid = true;                 break;
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

		//std::sort(m_queue.begin(), m_queue.end(), [&](RenderItem& a, RenderItem& b) {
		//	return Sort(a, b);
		//});

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

					Renderer::DrawMesh(draw->Transform, draw->Mesh);
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
		BeginSceneOP* op = m_pool.alloc<BeginSceneOP>();
		op->Scene  = nullptr;
		op->Camera = camera;
		op->Target = target;

		m_queue.emplace_back(0, RenderOP::BEGIN_SCENE, op);
	}

	void QueuedRenderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target)
	{
		BeginSceneOP* op = m_pool.alloc<BeginSceneOP>();
		op->Scene  = scene;
		op->Camera = nullptr;
		op->Target = target;

		m_queue.emplace_back(0, RenderOP::BEGIN_SCENE, op);
	}

	void QueuedRenderer::BeginShadowCast(
		Light* light)
	{
		BeginShadowOP* op = m_pool.alloc<BeginShadowOP>();
		op->Light = light;

		m_queue.emplace_back(0, RenderOP::BEGIN_SHADOW, op);
	}

	void QueuedRenderer::EndScene() {
		m_queue.emplace_back(0, RenderOP::END_SCENE, nullptr);
	}

	void QueuedRenderer::EndShadowCast() {
		m_queue.emplace_back(0, RenderOP::END_SHADOW, nullptr);
	}

	void QueuedRenderer::DrawMesh(
		const Transform* transform,
		const Mesh* mesh)
	{
		DrawMeshOP* op = m_pool.alloc<DrawMeshOP>();
		op->Transform = transform;
		op->Mesh = mesh;

		m_queue.emplace_back(0, RenderOP::DRAW_MESH, op);
	}

	bool QueuedRenderer::Sort(
		RenderItem& a, 
		RenderItem& b)
	{
		int alayer    = GetOrder(a.Order, Bits::LAYER);
		int acamera   = GetOrder(a.Order, Bits::CAMERA);
		int atransp   = GetOrder(a.Order, Bits::TRANSPARENCY);
		int adeoth    = GetOrder(a.Order, Bits::DEPTH);
		int amaterial = GetOrder(a.Order, Bits::MATERIAL);

		int blayer    = GetOrder(a.Order, Bits::LAYER);
		int bcamera   = GetOrder(a.Order, Bits::CAMERA);
		int btransp   = GetOrder(a.Order, Bits::TRANSPARENCY);
		int bdeoth    = GetOrder(a.Order, Bits::DEPTH);
		int bmaterial = GetOrder(a.Order, Bits::MATERIAL);

		return alayer     < blayer
			|| acamera   < bcamera
			|| atransp   < btransp
			|| adeoth    < bdeoth
			|| amaterial < bmaterial;
	}

	int QueuedRenderer::GetBits(
		int64_t order,
		int k,
		int p)
	{
		return (((1 << k) - 1) & (order >> (p - 1)));
	}

	int QueuedRenderer::GetOrder(
		int64_t order,
		Bits bits)
	{
		switch (bits) {
		case Bits::LAYER:        return GetBits(order, val(Bits::CAMERA), val(Bits::LAYER));
		case Bits::CAMERA:       return GetBits(order, val(Bits::TRANSPARENCY), val(Bits::CAMERA));
		case Bits::TRANSPARENCY: return GetBits(order, val(Bits::DEPTH), val(Bits::TRANSPARENCY));
		case Bits::DEPTH:        return GetBits(order, val(Bits::MATERIAL), val(Bits::DEPTH));
		case Bits::MATERIAL:     return GetBits(order, sizeof(int64_t), val(Bits::MATERIAL));
		default:                 return -1;
		}
	}
}
}
