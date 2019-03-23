#pragma once

#include "iw/engine/Layer.h"

#include "iw/renderer/Mesh.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/ShaderProgram.h"

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
		IwRenderer::ShaderProgram* shader;
		IwRenderer::Device* device;
		iwm::matrix4 transform;

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
