#pragma once

#include "iw/engine/Layer.h"

#include "iw/renderer/Mesh.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"

namespace IwEngine {
	struct Mesh {
		IwRenderer::VertexArray* Vertices;
		IwRenderer::IndexBuffer* Indices;
		unsigned int Count;
	};

	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		iwm::vector3 pos;
		iwm::vector3 vel;
		float rot;
		std::vector<Mesh> model;
		IwRenderer::Device* device;
		IwRenderer::Pipeline* pipeline;
		iwm::matrix4 modelTransform;
		iwm::matrix4 viewTransform;
		iwm::matrix4 projTransform;

	public:
		EntityLayer();
		~EntityLayer();

		int Initilize() override;
		void Destroy() override;
		void Update() override;
		void ImGui() override;

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&  event);
		bool On(MouseWheelEvent&   event);
	};
}
