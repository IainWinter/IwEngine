#pragma once

#include "iw/engine/Layer.h"
#include "iw/entity/Space.h"
#include "iw/graphics/RenderQueue.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"

class GameLayer
	: public IwEngine::Layer
{
private:
	IwGraphics::ModelLoader loader;
	IwRenderer::IPipeline* pipeline;
	IwRenderer::IPipeline* pipeline_line;

	//temp
	IwGraphics::ModelData* QuadData;
	IwGraphics::Mesh* QuadMesh;
	IwGraphics::Mesh* LineMesh;

	iwm::vector3* verts;

public:
	GameLayer(
		IwEntity::Space& space,
		IwGraphics::Renderer& renderer);

	~GameLayer();

	int Initialize(
		IwEngine::InitOptions& options) override;

	void PostUpdate()  override;
	void FixedUpdate() override;
	void ImGui()       override;
};
